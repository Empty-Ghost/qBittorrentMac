# macOS 26 Apple silicon dependency, stability, and efficiency upgrade plan

Status: in progress

Prepared: 2026-07-18

Scope: qBittorrent macOS 26 builds for Apple silicon (`arm64`), direct and vendored dependencies, developer tooling, packaging, runtime stability, and resource use

## Goal

Produce reproducible, native `arm64` builds for macOS 26 using the newest supportable long-term-support (LTS) releases where an upstream project offers an LTS channel, and the newest stable release otherwise. Improve stability and lower CPU, memory, disk, and energy use without changing BitTorrent behavior. macOS 26 and Apple silicon are deliberate product requirements; Intel Macs and earlier macOS releases are outside this plan rather than being dropped silently.

Implementation began on 2026-07-18. Progress and any revised decisions are recorded below.

## Implementation decisions and progress

- Default to one latest selected version in build and CI configuration instead of maintaining older compatibility lanes. Older non-macOS configurations may be removed as the macOS-only build direction becomes concrete.
- Target macOS 26.0 and Apple silicon exclusively. Release artifacts, CI, dependency builds, signing, notarization, and validation must use `arm64`; do not produce Intel (`x86_64`) or universal binaries.
- Selected the public latest-stable Qt path (6.11.1) for the primary macOS build. Qt 6.8 LTS is not the default because its newest patches are not publicly available.
- Selected libtorrent 2.1.0 and Boost 1.91.0 as the only primary macOS dependency pair. Compatibility lanes are no longer a prerequisite.
- Selected Node.js 24.18.0 LTS with npm 11.12.1 and Python 3.14.6 with uv 0.11.29 for deterministic developer-tool environments.
- Phase 1.2 is complete: exact JavaScript tool versions, an immutable qBittorrent ESLint plugin revision, a committed npm lock file, `npm ci`, and npm update automation were added and validated.
- Phase 1.3 is complete for the search engine: a committed uv lock file, pinned uv setup, `uv sync --locked`, and Python update automation were added and validated with Python 3.14.6. The search engine runtime floor remains Python 3.9 for now; build-tool selection does not silently raise an end-user runtime requirement.
- The primary macOS CI matrix now uses only Qt 6.11.1, libtorrent 2.1.0, Boost 1.91.0, OpenSSL 3.5.7, and zlib 1.3.2. Boost downloads are checksum-verified and the libtorrent tag is verified against its selected commit.
- The CMake default and committed presets now require macOS 26.0 and `arm64`; an explicit `x86_64` configuration fails during configure. The former universal release preset was replaced with a native Apple silicon release preset.
- Phase 1.1 is complete: `DEPENDENCIES.md` records selected versions, roles, sources, hashes, licenses, support policy, architectures, ownership, and update procedure. Build-only CMake, Ninja, and ccache archives may remain universal where that is the verified upstream distribution, but CI executes them natively on `arm64` and never bundles them. Syft uses its native `arm64` archive, generates a CycloneDX SBOM from each final app bundle, and retains it with CI artifacts. Qt 6.11.1 archives are retained and verified against a committed SHA-256 manifest using exact aqtinstall 3.3.0 and py7zr 1.1.3 versions; packaging accepts only their `arm64` slices.
- Local validation completed with CMake 4.4.0, Qt 6.11.1, libtorrent 2.1.0, and zlib 1.3.2: the full GUI bundle built and all 19 native tests passed. Web UI tests/lint/format and search-engine type/lint/build checks also passed.
- Release dependency builds no longer use Homebrew bottles: CI builds checksum-verified OpenSSL 3.5.7 and zlib 1.3.2 sources as static `arm64` libraries with a macOS 26.0 deployment target. The packaging job recursively rejects non-`arm64` Mach-O files, deployment requirements above macOS 26.0, and dependencies from `/opt/homebrew` or `/usr/local`; hosted clean-machine execution remains pending.
- Phase 1.4 now targets GitHub's `macos-26` Arm64 runner, verifies the host architecture, macOS major version, and Xcode major version, and keeps every executable third-party action pinned to an immutable commit. A next-Xcode forward-compatibility lane remains open.
- Local platform validation completed on macOS 26.5.2 with Xcode 26.6 and CMake 4.4.0: the GUI and no-GUI bundles built, all 19 native tests passed, executable and bundle metadata declare macOS 26.0, the packaging loop validated 63 thin `arm64` Mach-O files, both version commands run after their required local deployment step, and an Intel configure attempt fails as intended.
- The macOS sleep inhibitor now owns an explicit valid/invalid assertion state, ignores repeated transitions, retries failed releases, and releases an active assertion during destruction. Deterministic platform-call tests cover creation failure, invalid IDs, release failure, repeated calls, and destruction; live sleep/`pmset` validation remains pending.
- All five macOS Objective-C++ sources now compile with ARC. Dock tile and status-item teardown is explicit, notification/menu allocations are ARC-managed, the application builds, all 20 native tests pass, and the Clang static analyzer reports no findings in those sources. Instruments leak/allocation stress testing remains pending.
- Dock and menu-bar speed surfaces now compare their final formatted labels before redrawing. The Dock content view is detached while its feature is disabled, hidden status items reject speed updates, preference changes still refresh immediately, and a macOS regression test covers distinct raw rates that render identically. The application builds, all 21 native tests pass, and the changed Objective-C++ sources are analyzer-clean. Instruments before/after measurement and evaluation of a coarser independent UI cadence remain pending.
- Sleep-inhibitor aggregation now reacts immediately to torrent add/remove, finish, metadata, start/stop, I/O error, and storage-moving transitions instead of scanning every torrent every 60 seconds. A ten-minute very-coarse reconciliation timer remains as a safety net. Live sleep/`pmset` validation remains pending.
- Background timer audit and first coalescing pass are complete. Deferred settings/log/RSS writes, free-space checks, seeding limits, resume-data saves, additional-tracker list refreshes, DNS checks, RSS feed refreshes, watched-folder retries, and wake detection now explicitly use coarse or very-coarse timers. Automatic update checks wait five minutes after startup, then use a 24-hour very-coarse schedule; interactive and correctness-sensitive timeouts remain precise. Instruments wakeup measurement remains pending.
- Release hardening now has an explicitly manual, secret-gated CI path. It imports a Developer ID Application identity into a temporary keychain, signs nested code and the app inside-out with Hardened Runtime and a secure timestamp, signs and notarizes the DMG with an App Store Connect API key, staples and validates the ticket, runs Gatekeeper assessment, and deletes credentials even after failure. Ordinary push and pull-request artifacts remain ad-hoc signed but exercise Hardened Runtime signing and nested-signature validation. A credentialed hosted release run and clean-machine installation remain pending.

## Version policy

“Latest LTS” is not a universal version category. Apply these rules:

1. Use the latest patch of the newest upstream LTS branch for Qt, OpenSSL, and Node.js.
2. Use the latest stable release for projects without an LTS program: libtorrent, Boost, zlib, CMake, Python, Lua, and developer tools.
3. Keep minimum supported versions separate from versions used to build release artifacts. This plan deliberately sets the product minimum to macOS 26.0; do not raise it further merely because CI uses a newer release.
4. Pin release and CI inputs to exact versions and integrity hashes. Test only the selected latest build versions unless a compatibility lane is explicitly restored.
5. Upgrade one dependency family at a time and keep each change independently reversible.
6. Do not use release candidates, nightly builds, or Homebrew `HEAD` revisions for release artifacts.

## Current dependency findings and proposed targets

Versions below were verified against upstream or package-registry data on 2026-07-18.

| Component | Repository state | Proposed release target | Support decision |
| --- | --- | --- | --- |
| Qt | Minimum 6.6.0; macOS CI 6.11.1 | 6.11.1 stable | Qt 6.8.8 LTS patches are commercial-only. The public latest-stable release is the single primary macOS target. |
| OpenSSL | Minimum 3.0.2; macOS CI builds verified 3.5.7 sources | 3.5.7 LTS | OpenSSL 3.5 is supported through 2030-04-08. The release build uses static libraries instead of a floating Homebrew formula. |
| Node.js | CI pins 24.18.0 and npm 11.12.1 | 24.18.0 LTS | Update the exact patch through Dependabot-reviewed lock-file changes. |
| libtorrent | Minimum 1.2.19 or 2.0.10; macOS CI pins 2.1.0 | 2.1.0 stable for primary macOS artifacts | No LTS channel. Older compatibility lanes were removed by project direction. |
| Boost | Minimum 1.76; macOS CI pins 1.91.0 | 1.91.0 stable | The selected archive is verified by its upstream SHA-256. |
| zlib | Minimum 1.2.11; macOS CI builds verified 1.3.2 sources | 1.3.2 stable | No LTS channel. Rebuild the static library as `arm64` with a macOS 26 deployment target. |
| CMake | Minimum 3.16; macOS CI installs verified 4.4.0 | 4.4.0 stable for CI and local presets | CMake is a build tool, not a shipped library. For Apple builds, separately consider raising the macOS-only floor to 3.21.1, which Qt documents for Apple deployment. |
| Python | `INSTALL` says 3.13+, search runtime floor is 3.9; search tooling CI pins 3.14.6 | 3.14.6 stable for tooling | Python has no LTS release. The runtime floor remains separate from the build-tool version. |
| Lua | Vendored and excluded from normal review paths | Evaluate 5.5.0 stable; retain/update to 5.4.8 first if Lua 5.5 compatibility is incomplete | This is an ABI/API migration, not a patch-only update. It needs a plugin compatibility test before adoption. |
| LuaBridge | Vendored; application reports a 3.0 version | Audit against the upstream LuaBridge3 tag in a dedicated vendor change | Upstream’s visible tag is still a release candidate. Do not replace known-good vendored code with a pre-release solely to increase a version number. |
| expected-lite | Vendored single header | 0.10.0 stable, if local modifications and API compatibility permit | Compare provenance and local changes before replacing the header. |

Primary references:

- [Qt release and LTS policy](https://doc.qt.io/qt-6.8/qt-releases.html)
- [Qt for macOS supported configurations](https://doc.qt.io/qt-6.8/macos.html)
- [OpenSSL roadmap](https://openssl-library.org/roadmap/index.html)
- [OpenSSL downloads](https://openssl-library.org/source/)
- [Node.js release schedule](https://nodejs.org/en/about/previous-releases)
- [libtorrent releases](https://github.com/arvidn/libtorrent/releases)
- [Boost latest release](https://www.boost.org/releases/latest/)
- [zlib current release](https://zlib.net/)
- [CMake downloads](https://cmake.org/download/)
- [Python source releases](https://www.python.org/downloads/source/)
- [Lua version history](https://www.lua.org/versions.html)

## Phase 1: establish reproducible dependency inputs

### 1.1 Create a dependency manifest

Add one human-readable manifest containing:

- release version, minimum supported version, source URL, SHA-256, license, and support/EOL date;
- whether the component is linked into the app, bundled in the app, used at runtime, or build/test-only;
- `arm64` architecture availability and confirmation that no `x86_64` slice is required or shipped;
- the owner and update procedure for every vendored component;
- a note when no upstream LTS channel exists.

Generate an SBOM for each release artifact and retain it with CI artifacts.

### 1.2 Make JavaScript builds deterministic

The Web UI uses wildcard versions and deliberately ignores `package-lock.json`. Change this in a standalone tooling change:

1. Set `engines.node` to the selected Node 24 LTS range.
2. Replace `*` declarations with compatible major ranges or exact versions.
3. Stop ignoring `package-lock.json`, generate it with the selected npm release, and commit it.
4. Replace `npm install` in CI with `npm ci`.
5. Add Dependabot’s npm ecosystem for `/src/webui/www`.
6. Keep the GitHub tarball dependency for the qBittorrent ESLint plugin pinned to an immutable commit digest if upstream does not publish a registry release.

Candidate tool versions as of the audit date:

| Package | Version |
| --- | --- |
| `@eslint/js` | 10.0.1 |
| `@stylistic/eslint-plugin` | 5.10.0 |
| `eslint` | 10.7.0 |
| `eslint-plugin-html` | 8.1.4 |
| `eslint-plugin-prefer-arrow-functions` | 3.10.1 |
| `eslint-plugin-regexp` | 3.1.1 |
| `eslint-plugin-unicorn` | 72.0.0 |
| `happy-dom` | 20.10.6 |
| `html-validate` | 11.5.6 |
| `js-beautify` | 2.0.3 |
| `prettier` | 3.9.5 |
| `stylelint` | 17.14.0 |
| `stylelint-config-standard` | 40.0.0 |
| `stylelint-order` | 8.1.1 |
| `vitest` | 4.1.10 |

Run the entire format/lint/test suite after each major-version group because several of these upgrades contain rule or configuration changes.

### 1.3 Make Python tooling deterministic

1. Decide whether the supported search-engine floor is Python 3.9 or 3.13, then make `INSTALL`, `pyproject.toml`, runtime validation, and CI agree.
2. Test the chosen minimum and Python 3.14.6 in separate CI jobs.
3. Stop ignoring `uv.lock`, commit it, and run `uv sync --locked` in CI.
4. Pin the `uv` installer/version rather than installing an unconstrained latest release.
5. Add Dependabot or Renovate coverage for Python and lock-file refreshes.

Candidate development-tool versions as of the audit date:

| Package | Version |
| --- | --- |
| `bandit` | 1.9.4 |
| `isort` | 8.0.1 |
| `mypy` | 2.3.0 |
| `pycodestyle` | 2.14.0 |
| `pydoclint` | 0.9.1 |
| `pyflakes` | 3.4.0 |
| `pyright` | 1.1.411 |
| `pytest` | 9.1.1 |
| `rust-just` | 1.56.0 |
| `uv` | 0.11.29 |

### 1.4 Pin CI and Homebrew inputs

- Pin GitHub Actions to immutable commit SHAs while retaining version comments for Dependabot.
- Pin the primary runner to a stable macOS 26 image with the selected Xcode 26 release; keep one allowed-to-fail job on the next available macOS/Xcode preview image.
- Do not rely on a floating `openssl@3` formula for the LTS requirement. Use `openssl@3.5` or build verified 3.5.7 sources.
- Capture formula versions and bottle hashes in the build log.
- Add CMake presets for native Apple silicon debug, sanitizer, and release builds with `CMAKE_OSX_ARCHITECTURES=arm64` and `CMAKE_OSX_DEPLOYMENT_TARGET=26.0`.

## Phase 2: upgrade dependency families

Perform these as separate, reviewable changes in this order.

### 2.1 Build tools and lock files

- Validate CMake 4.4 policies and remove warnings before changing the minimum version.
- Pin Ninja and ccache in CI.
- Introduce the dependency manifest, lock files, and immutable download hashes.
- Verify clean builds from a new checkout with no pre-existing Homebrew link paths.

### 2.2 OpenSSL and zlib

- Build against OpenSSL 3.5.7 LTS and zlib 1.3.2.
- Run Web UI TLS tests, certificate loading, tracker HTTPS, search downloads, and malformed compressed-response tests.
- Verify the app bundle does not reference `/opt/homebrew` or `/usr/local` with `otool -L` and recursive bundle inspection.
- Confirm the expected OpenSSL provider modules and CA bundle are available in the distributed app.

### 2.3 Boost and libtorrent

- Make libtorrent 2.1.0 plus Boost 1.91.0 the primary macOS configuration.
- Do not restore older libtorrent compatibility jobs unless a concrete macOS release requirement calls for them.
- Exercise resume-data migration, v1/v2/hybrid torrents, magnet metadata, WebTorrent, proxy/I2P paths, sparse files, moving storage, and unclean shutdown recovery.
- Benchmark the available libtorrent disk backends on APFS. Select a new default only if it improves measured throughput or energy without increasing memory, corruption risk, or compatibility failures.
- Treat any switch to the experimental `pread` backend as opt-in until upstream calls it stable and the soak tests pass.

### 2.4 Qt

- Use public open-source Qt 6.11.1 as the selected release path.
- Do not label Qt 6.11.1 as LTS.
- Test Qt Core, CorePrivate, Network, SQL, XML, Widgets, SVG, LinguistTools, translations, and image-format plugins.
- Re-test native menus, notifications, Dock behavior, menu-bar item, file associations, drag and drop, Finder reveal, dark mode, Retina scaling, accessibility, and multi-display window restoration.
- Keep the minimum Qt setting independent of the release pin until the compatibility matrix justifies raising it.

### 2.5 Python, Node.js, and development tools

- Upgrade Node and JavaScript packages in compatible major-version groups.
- Upgrade Python tooling from the committed lock file.
- Test search plugins on clean Apple silicon user accounts with no shell-initialization files.
- Validate Python discovery without repeatedly modifying `PATH`; prefer a validated absolute executable and cover the Apple silicon `/opt/homebrew` installation without adding Intel `/usr/local` as a supported configuration.

### 2.6 Vendored libraries

The repository configuration excludes vendored paths from ordinary review, so perform this as an explicit vendor audit rather than casually editing excluded files.

- Record the exact current Lua, LuaBridge, and expected-lite provenance, commit/tag, checksum, license, and local patches.
- Upgrade expected-lite separately.
- First test the latest Lua 5.4 patch. Test Lua 5.5 in a separate compatibility branch because its VM/API transition requires all bundled and third-party plugins to be revalidated.
- Run plugin timeouts, malformed scripts, repeated load/unload, exception bridging, memory limits, and shutdown under AddressSanitizer and the Clang static analyzer.
- Update runtime version reporting from source metadata rather than a manually duplicated constant where possible.

## Phase 3: macOS stability improvements

### Priority 0: fix sleep-inhibitor lifecycle and state handling

Relevant code: `src/gui/powermanagement/inhibitormacos.*` and `src/gui/mainwindow.cpp`.

Current behavior scans every torrent every 60 seconds and maintains an IOKit assertion ID with no macOS-specific destructor or explicit invalid-ID state.

Plan:

1. Add unit-testable state transitions for idle to busy, busy to idle, repeated calls, assertion-creation failure, assertion-release failure, and destruction while busy.
2. Release only a valid assertion and clear the ID after a successful release. Guarantee cleanup in the inhibitor destructor.
3. Evaluate replacing deprecated `IOPMAssertionCreateWithName` with `NSProcessInfo` activity tokens, using `NSActivityIdleSystemSleepDisabled`, or a current IOPM properties API. Keep only one active token/assertion.
4. Replace the unconditional full torrent scan with state-change-driven aggregation. Keep a coarse, low-frequency reconciliation timer only as a safety net.
5. Verify sleep, wake, lid close/open, network reconnection, paused sessions, moving torrents, and app shutdown while active.
6. Confirm in `pmset -g assertions` that no assertion remains after transfers stop or the app exits.

Impact: prevents stale sleep assertions, failed release calls, unnecessary periodic work, and possible assertion leaks.

### Priority 0: make Objective-C ownership explicit

Relevant code: `src/gui/macutilities.mm`, `src/gui/macosdockbadge/*`, and `src/gui/macosstatusitem/*`.

The CMake target does not explicitly enable ARC, while several `alloc/init` objects are stored or handed off without a matching release. Examples include notification content, Dock badge views, shadows, status items, menus, and menu items.

Plan:

1. Confirm the effective Objective-C++ compile mode in verbose CI output.
2. Run the Clang static analyzer, Instruments Leaks/Allocations, and repeated notification/menu-bar enable-disable tests.
3. Choose one ownership model for all macOS Objective-C++ files:
   - enable ARC and remove manual `autorelease`/ownership calls safely; or
   - retain manual reference counting and add complete `dealloc`/destructor cleanup.
4. On teardown, clear the Dock tile content view, remove the `NSStatusItem` from `NSStatusBar`, release owned views/menus, and avoid updating objects after application shutdown begins.
5. Add autorelease pools around background Objective-C work and tests that send thousands of notifications or UI updates without memory growth.

Impact: removes concrete leak risks and reduces shutdown-time use-after-free risk.

### Priority 1: reduce Dock and menu-bar redraw work

The Dock and status-item code receives raw byte rates on every session stats update. Exact rates frequently change even when the displayed formatted value does not.

Plan:

- Cache the final formatted download/upload labels and redraw only when visible text changes.
- Do no work when the corresponding feature is disabled.
- Consider a 2–5 second coarse UI cadence for Dock/menu-bar speeds, independent of the torrent engine refresh cadence.
- Cache immutable drawing attributes and validate Retina rendering at 1x and 2x.
- Measure wakeups and main-thread time before and after; retain the change only if the UI remains responsive and metrics improve.

### Priority 1: make background and periodic work energy-aware

Apple recommends eliminating polling where events exist and allowing timer coalescing where precision is unnecessary.

- Inventory every repeating/single-shot timer active while the app is idle, hidden, or paused.
- Stop UI-only timers when their window/tab is not visible.
- Use `Qt::CoarseTimer` or `Qt::VeryCoarseTimer` for update checks, RSS maintenance, persistence batching, and other noninteractive work where timing permits.
- Batch settings, resume-data, and log writes, while preserving crash-recovery guarantees.
- Avoid network update checks immediately at launch when restoring a large session; defer them until startup settles and use background quality of service.
- Observe Low Power Mode and thermal-state changes only after profiling demonstrates useful savings and behavior remains predictable.

References: [Apple timer guidance](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/power_efficiency_guidelines_osx/Timers.html) and [Apple energy best practices](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/power_efficiency_guidelines_osx/BestPractices.html).

### Priority 1: harden Finder, notification, and shutdown integration

- Preserve a regression test for the historical Qt event-loop crash associated with Finder reveal. Apple documents `activateFileViewerSelectingURLs:` as safe from any thread, but do not simplify the workaround without a stress test on the selected Qt release and macOS 26.
- Use immutable URL collections for dispatched Finder work and an explicit quality-of-service class.
- Handle notification authorization and delivery errors on a safe queue, with bounded logging.
- Validate file and magnet associations using modern Uniform Type Identifier APIs on macOS 26.
- Stress quit/restart/sleep actions while file moves, resume-data writes, notifications, and Finder operations are in flight.

### Priority 2: enforce the macOS 26 Apple silicon platform target

- Set `CMAKE_OSX_DEPLOYMENT_TARGET` and `LSMinimumSystemVersion` to `26.0`, and make CI fail if either value drifts.
- Set `CMAKE_OSX_ARCHITECTURES=arm64` for all macOS presets and dependency builds. Produce one native Apple silicon artifact; do not build or merge an `x86_64` slice.
- Build with the selected Xcode 26 toolchain supported by the selected Qt line, plus one allowed-to-fail lane for the next available Xcode release.
- Recursively inspect the final app, frameworks, plugins, and helper executables with `lipo -archs`, `file`, and `otool`; fail packaging if any shipped Mach-O file is not `arm64`, contains an `x86_64` slice, or requires a macOS version newer than 26.0. Separately require the main executable and bundle metadata to declare 26.0 as their minimum.
- Run clean-machine tests on macOS 26 Apple silicon and verify that launch does not invoke Rosetta or depend on an undeclared Intel-only helper.
- Move Apple framework links to the targets that actually use them and remove unused links only after link-map verification.
- Produce release builds with Hardened Runtime, a secure timestamp, Developer ID signing, notarization, and stapling.
- Verify entitlements, nested code signatures, bundle identifiers, plugin paths, translations, CA certificates, and absence of writable executable content.

## Phase 4: measurement and verification

### Baseline scenarios

Capture results before implementation on macOS 26 Apple silicon. Use the same hardware class, power state, and OS build for comparable before/after measurements:

1. clean launch with no torrents;
2. restored session with 100, 1,000, and a practical maximum number of torrents;
3. idle, paused, downloading, seeding, checking, moving, and error states;
4. main window visible, hidden, minimized, and menu-bar-only operation;
5. AC power, battery, Low Power Mode, sleep/wake, and network loss/recovery;
6. Web UI disabled/enabled with idle and active clients;
7. search and RSS disabled/enabled;
8. 8-hour active soak and 24-hour idle soak;
9. repeated launch/quit and forced termination followed by recovery.

### Metrics

Record median and high-percentile values, not a single snapshot:

- idle and active CPU time;
- wakeups per second and timer fires;
- resident, dirty, and compressed memory;
- allocation growth after repeated actions;
- disk bytes/writes and resume-data write frequency;
- network bytes unrelated to torrent payload;
- launch-to-responsive time and main-thread hangs;
- Energy Impact/Power Profiler results;
- assertion lifetime and count;
- crash-free soak duration and clean recovery rate.

Use Instruments Time Profiler, Allocations, Leaks, System Trace, File Activity, Network, and Power Profiler. Add sanitizer builds for AddressSanitizer and UndefinedBehaviorSanitizer; run ThreadSanitizer in a separate compatible configuration. Apple’s current guidance is to establish baselines and retest after each change: [Testing and performance](https://developer.apple.com/documentation/technologyoverviews/testing-and-performance).

### Acceptance gates

A dependency or optimization change is ready only when:

- all unit, integration, Web UI, and search tests pass;
- the app passes a clean-machine bundle test with Homebrew temporarily unavailable;
- no new sanitizer, analyzer, leak, hang, or signing finding is introduced;
- sleep assertions always match active work and are cleared at shutdown;
- no macOS 26 or `arm64` regression is found;
- idle CPU, wakeups, memory, and disk writes do not regress beyond normal run-to-run variance;
- a claimed resource improvement is supported by comparable before/after traces;
- rollback instructions and the previous known-good pins are documented.

Suggested performance gates after baselines establish realistic variance:

- no statistically meaningful idle CPU regression;
- at least a 20% reduction in idle wakeups for timer/polling changes;
- no sustained memory growth in 1,000-cycle notification, Dock, menu-bar, and Finder tests;
- no stale power assertion within one reconciliation interval after the last qualifying transfer stops.

## Proposed implementation sequence

1. Add dependency manifest, checksums, SBOM generation, lock files, and exact CI inputs.
2. Add baseline scripts/tests and capture performance traces before code changes.
3. Move OpenSSL to 3.5.7 LTS and zlib to 1.3.2.
4. Standardize the macOS build on libtorrent 2.1.0 and Boost 1.91.0.
5. Use the public latest-stable Qt release path and keep its exact version pinned in the Qt matrix.
6. Reconcile and lock Python, Node.js, npm, and Python development tools.
7. Audit and update vendored libraries in isolated changes.
8. Fix macOS Objective-C ownership and sleep-inhibitor lifecycle.
9. Implement measured Dock/menu/timer reductions.
10. Enforce `arm64`-only macOS 26 packaging, then harden signing, notarization, and clean-machine verification.
11. Run full compatibility, sanitizer, sleep/wake, and soak gates before adopting the new release pins.

## Files expected to change during implementation

- `CMakeLists.txt`, `cmake/Modules/CheckPackages.cmake`, and macOS CMake presets;
- `INSTALL` and dependency/version documentation;
- `.github/workflows/ci_macos.yaml`, Python/Web UI workflows, and `.github/dependabot.yml`;
- `.gitignore`, `src/webui/www/.gitignore`, and `src/searchengine/.gitignore` for committed lock files;
- `src/webui/www/package.json` and a committed `package-lock.json`;
- `src/searchengine/nova3/pyproject.toml` and a committed `uv.lock`;
- explicit vendored dependency files in isolated vendor changes;
- `src/gui/powermanagement/inhibitormacos.*` and power-management orchestration;
- `src/gui/macutilities.mm`, `src/gui/macosdockbadge/*`, and `src/gui/macosstatusitem/*`;
- `src/app/CMakeLists.txt` and `dist/mac/Info.plist` for deployment metadata;
- macOS integration, lifecycle, and performance tests.

## Rollback strategy

- Keep every dependency family and macOS optimization in a separate commit/change set.
- Retain the previous version manifest and hashes for at least one release cycle.
- Make disk-backend and UI-refresh behavior changes independently reversible.
- Never downgrade user configuration or resume data during rollback.
- Test opening data written by the upgraded build with the previous supported build before release.
