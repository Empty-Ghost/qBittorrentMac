/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2025  Mike Tzou (Chocobo1)
 * Copyright (C) 2011  Vladimir Golovnev <glassez@yandex.ru>
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

#include "inhibitormacos.h"

#include <QScopeGuard>

namespace
{
    IOReturn createAssertion(const CFStringRef name, IOPMAssertionID *assertionID)
    {
        return ::IOPMAssertionCreateWithName(kIOPMAssertionTypeNoIdleSleep, kIOPMAssertionLevelOn
            , name, assertionID);
    }

    IOReturn releaseAssertion(const IOPMAssertionID assertionID)
    {
        return ::IOPMAssertionRelease(assertionID);
    }
}

InhibitorMacOS::InhibitorMacOS()
    : InhibitorMacOS {createAssertion, releaseAssertion}
{
}

InhibitorMacOS::InhibitorMacOS(const AssertionCreateFunction createAssertion
    , const AssertionReleaseFunction releaseAssertion)
    : m_createAssertion {createAssertion}
    , m_releaseAssertion {releaseAssertion}
{
}

InhibitorMacOS::~InhibitorMacOS()
{
    requestIdle();
}

bool InhibitorMacOS::requestBusy()
{
    if (m_assertionID != kIOPMNullAssertionID)
        return true;

    const CFStringRef assertName = tr("PMMacOS", "qBittorrent is active").toCFString();
    [[maybe_unused]] const auto assertNameGuard = qScopeGuard([&assertName] { ::CFRelease(assertName); });

    IOPMAssertionID assertionID = kIOPMNullAssertionID;
    if ((m_createAssertion(assertName, &assertionID) != kIOReturnSuccess)
        || (assertionID == kIOPMNullAssertionID))
    {
        return false;
    }

    m_assertionID = assertionID;
    return true;
}

bool InhibitorMacOS::requestIdle()
{
    if (m_assertionID == kIOPMNullAssertionID)
        return true;

    if (m_releaseAssertion(m_assertionID) != kIOReturnSuccess)
        return false;

    m_assertionID = kIOPMNullAssertionID;
    return true;
}
