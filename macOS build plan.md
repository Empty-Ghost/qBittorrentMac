macOS build plan
1. Install missing dependencies
Use the Homebrew packages corresponding to the repository’s macOS CI configuration:
brew install \
  ninja \
  qtbase qtdeclarative qtsvg qttools qtimageformats qttranslations \
  libtorrent-rasterbar \
  zlib
libtorrent-rasterbar will also install the required Boost libraries. Optional for faster repeat builds:
brew install ccache
Expected versions from current Homebrew metadata:
Component	Available	Project requirement
Qt	6.11.1	≥ 6.6
libtorrent	2.1.0	CI tests 2.1.0
Boost	1.90.0	≥ 1.76
OpenSSL	3.6.3, installed	≥ 3.0.2
zlib	1.3.2	≥ 1.2.11
CMake	4.4.0, installed	≥ 3.16
Python	3.14.6, installed	≥ 3.13
Ninja	1.13.2	Recommended build engine

These requirements come from [CMakeLists.txt (line 10)](/Users/justin/Development/qBittorrentMac/CMakeLists.txt:10), [INSTALL (line 4)](/Users/justin/Development/qBittorrentMac/INSTALL:4), and the project’s [macOS CI workflow (line 16)](/Users/justin/Development/qBittorrentMac/.github/workflows/ci_macos.yaml:16).
2. Configure an ARM64 development build
cmake \
  -S . \
  -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_PREFIX_PATH="/opt/homebrew;/opt/homebrew/opt/openssl@3;/opt/homebrew/opt/zlib" \
  -DTESTING=ON \
  -DVERBOSE_CONFIGURE=ON
Review the configure summary and confirm that it finds:
Qt Core, Network, SQL, XML, LinguistTools, GUI, Widgets, and SVG
libtorrent-rasterbar 2.1
Boost
OpenSSL 3
zlib
3. Compile and test
cmake --build build
cmake --build build --target check
Confirm that the generated executable starts and reports its version:
build/qbittorrent.app/Contents/MacOS/qbittorrent -v
Then launch the GUI:
open build/qbittorrent.app
4. Create a locally distributable app
Bundle Qt and other required libraries:
macdeployqt build/qbittorrent.app -no-strip
Apply an ad-hoc signature for local testing:
xattr -cr build/qbittorrent.app
codesign --force --deep --sign - build/qbittorrent.app
codesign --verify --deep --strict --verbose build/qbittorrent.app
Create the disk image:
hdiutil create \
  -fs HFS+ \
  -srcfolder build/qbittorrent.app \
  -volname qBittorrent \
  build/qbittorrent.dmg
5. Final verification
Test on a clean macOS account or another Apple Silicon Mac:
Open the DMG and copy the app to Applications.
Ensure the app starts without Homebrew paths being required.
Open a .torrent file and a magnet: URL.
Check Web UI startup and the Python-based search feature.
Inspect bundled dependencies with otool -L.
For distribution to other users, replace the ad-hoc signature with a Developer ID Application certificate and notarize the DMG using Apple’s notarytool. Xcode already supplies the signing and notarization tools; only Apple developer credentials and certificates would be additional requirements.
