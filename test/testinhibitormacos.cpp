/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2026  The qBittorrent project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include <QObject>
#include <QTest>

#include "gui/powermanagement/inhibitormacos.h"

class TestInhibitorMacOS final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestInhibitorMacOS)

public:
    TestInhibitorMacOS() = default;

private slots:
    void init()
    {
        createResult = kIOReturnSuccess;
        releaseResult = kIOReturnSuccess;
        createdAssertionID = assertionID;
        createCount = 0;
        releaseCount = 0;
        releasedAssertionID = kIOPMNullAssertionID;
    }

    void testStateTransitions() const
    {
        InhibitorMacOS inhibitor {createAssertion, releaseAssertion};

        QVERIFY(inhibitor.requestIdle());
        QCOMPARE(releaseCount, 0);

        QVERIFY(inhibitor.requestBusy());
        QCOMPARE(createCount, 1);
        QVERIFY(inhibitor.requestBusy());
        QCOMPARE(createCount, 1);

        QVERIFY(inhibitor.requestIdle());
        QCOMPARE(releaseCount, 1);
        QCOMPARE(releasedAssertionID, assertionID);
        QVERIFY(inhibitor.requestIdle());
        QCOMPARE(releaseCount, 1);
    }

    void testCreateFailure() const
    {
        createResult = kIOReturnError;
        {
            InhibitorMacOS inhibitor {createAssertion, releaseAssertion};
            QVERIFY(!inhibitor.requestBusy());
            QCOMPARE(createCount, 1);
        }
        QCOMPARE(releaseCount, 0);
    }

    void testCreateWithInvalidID() const
    {
        createdAssertionID = kIOPMNullAssertionID;
        {
            InhibitorMacOS inhibitor {createAssertion, releaseAssertion};
            QVERIFY(!inhibitor.requestBusy());
            QCOMPARE(createCount, 1);
        }
        QCOMPARE(releaseCount, 0);
    }

    void testReleaseFailureCanBeRetried() const
    {
        InhibitorMacOS inhibitor {createAssertion, releaseAssertion};
        QVERIFY(inhibitor.requestBusy());

        releaseResult = kIOReturnError;
        QVERIFY(!inhibitor.requestIdle());
        QCOMPARE(releaseCount, 1);

        releaseResult = kIOReturnSuccess;
        QVERIFY(inhibitor.requestIdle());
        QCOMPARE(releaseCount, 2);
        QCOMPARE(releasedAssertionID, assertionID);
    }

    void testDestructorReleasesBusyAssertion() const
    {
        {
            InhibitorMacOS inhibitor {createAssertion, releaseAssertion};
            QVERIFY(inhibitor.requestBusy());
        }
        QCOMPARE(releaseCount, 1);
        QCOMPARE(releasedAssertionID, assertionID);
    }

    void testDestructorRetriesFailedRelease() const
    {
        {
            InhibitorMacOS inhibitor {createAssertion, releaseAssertion};
            QVERIFY(inhibitor.requestBusy());
            releaseResult = kIOReturnError;
            QVERIFY(!inhibitor.requestIdle());
            releaseResult = kIOReturnSuccess;
        }
        QCOMPARE(releaseCount, 2);
        QCOMPARE(releasedAssertionID, assertionID);
    }

private:
    static IOReturn createAssertion([[maybe_unused]] CFStringRef name, IOPMAssertionID *createdAssertionID)
    {
        ++createCount;
        if (createResult == kIOReturnSuccess)
            *createdAssertionID = TestInhibitorMacOS::createdAssertionID;
        return createResult;
    }

    static IOReturn releaseAssertion(const IOPMAssertionID assertionIDToRelease)
    {
        ++releaseCount;
        releasedAssertionID = assertionIDToRelease;
        return releaseResult;
    }

    static constexpr IOPMAssertionID assertionID = 42;
    static inline IOReturn createResult = kIOReturnSuccess;
    static inline IOReturn releaseResult = kIOReturnSuccess;
    static inline IOPMAssertionID createdAssertionID = assertionID;
    static inline int createCount = 0;
    static inline int releaseCount = 0;
    static inline IOPMAssertionID releasedAssertionID = kIOPMNullAssertionID;
};

QTEST_APPLESS_MAIN(TestInhibitorMacOS)
#include "testinhibitormacos.moc"
