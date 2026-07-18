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

#import "gui/macosdockbadge/badgeview.h"

class TestMacOSDockBadge final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestMacOSDockBadge)

public:
    TestMacOSDockBadge() = default;

private slots:
    void testFormattedRateChanges();
};

void TestMacOSDockBadge::testFormattedRateChanges()
{
    BadgeView *view = [[BadgeView alloc] init];

    QVERIFY(![view setRatesWithDownload:0 upload:0]);
    QVERIFY([view setRatesWithDownload:1000 upload:1000]);
    QVERIFY(![view setRatesWithDownload:1001 upload:1001]);
    QVERIFY([view setRatesWithDownload:5000 upload:5000]);
    QVERIFY([view setRatesWithDownload:0 upload:0]);
    QVERIFY(![view setRatesWithDownload:0 upload:0]);
}

QTEST_APPLESS_MAIN(TestMacOSDockBadge)

#include "testmacosdockbadge.moc"
