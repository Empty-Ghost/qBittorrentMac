/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2025  Dru Still <drustill4@gmail.com>
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

#import "itemview.h"

#include <QString>
#include "base/utils/misc.h"

namespace
{
    static NSString * const kUploadArrow = @"\u2191";
    static NSString * const kDownloadArrow = @"\u2193";
}

@interface ItemView ()

@property(strong) NSStatusItem *statusItem;

@property(strong) NSMenu *statusMenu;
@property(strong) NSMenuItem *statusStatsItem;
@end

@implementation ItemView

- (instancetype)init
{
    if ((self = [super init]))
    {
        NSImage *icon = [NSImage imageNamed:@"qbittorrent_mac"];
        [icon setSize:NSMakeSize(16, 16)];

        self.statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSSquareStatusItemLength];
        self.statusItem.button.image = icon;
        self.statusItem.button.imagePosition = NSImageOnly;
        self.statusItem.button.imageScaling = NSImageScaleProportionallyDown;
        self.statusItem.button.toolTip = @"qBittorrent";
        self.statusItem.button.title = @"";

        self.statusMenu = [[NSMenu alloc] init];

        const QString uploadString = Utils::Misc::friendlyUnit(0, true);
        const QString downloadString = Utils::Misc::friendlyUnit(0, true);

        NSString *uploadNSString = uploadString.toNSString();
        NSString *downloadNSString = downloadString.toNSString();

        self.statusStatsItem = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ %@ · %@ %@",
                                                                  kDownloadArrow, downloadNSString,
                                                                  kUploadArrow, uploadNSString] action:nil keyEquivalent:@""];
        self.statusStatsItem.action = nil;
        self.statusStatsItem.target = nil;
        [self.statusMenu addItem:self.statusStatsItem];
        [self.statusMenu addItem:[NSMenuItem separatorItem]];

        self.statusItem.menu = self.statusMenu;
    }
    return self;
}

- (void)dealloc
{
    self.statusItem.menu = nil;
    [[NSStatusBar systemStatusBar] removeStatusItem:self.statusItem];
}

- (BOOL)setRatesWithDownload:(int64_t)downloadRate upload:(int64_t)uploadRate
{
    if (!self.statusItem.visible)
        return NO;

    const QString uploadString = Utils::Misc::friendlyUnit(uploadRate, true);
    const QString downloadString = Utils::Misc::friendlyUnit(downloadRate, true);

    NSString *uploadNSString = uploadString.toNSString();
    NSString *downloadNSString = downloadString.toNSString();

    NSString *title = [NSString stringWithFormat:@"%@ %@ · %@ %@",
                       kDownloadArrow, downloadNSString,
                       kUploadArrow, uploadNSString];
    if ([self.statusStatsItem.title isEqualToString:title])
        return NO;

    self.statusStatsItem.title = title;

    return YES;
}

- (void)setVisible:(BOOL)visible
{
    self.statusItem.visible = visible;
}

@end
