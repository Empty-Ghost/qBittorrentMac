# macOS build plan

## 1. Install dependencies

Use the Homebrew packages corresponding to the repository’s macOS CI configuration:

```shell
brew install \
  ninja \
  qtbase qtdeclarative qtsvg qttools qtimageformats qttranslations \
  libtorrent-rasterbar \
  zlib
```

`libtorrent-rasterbar` also installs the required Boost libraries. Optionally install ccache for faster repeat builds:

```shell
brew install ccache
```

Expected versions from current Homebrew metadata:

| Component | Available | Project requirement |
| --- | --- | --- |
| Qt | 6.11.1 | 6.6 or newer |
| libtorrent | 2.1.0 | CI tests 2.1.0 |
| Boost | 1.90.0 | 1.76 or newer |
| OpenSSL | 3.6.3, installed | 3.0.2 or newer |
| zlib | 1.3.2 | 1.2.11 or newer |
| CMake | 4.4.0, installed | 3.16 or newer |
| Python | 3.14.6, installed | 3.13 or newer |
| Ninja | 1.13.2 | Recommended build engine |

These requirements come from `CMakeLists.txt`, `INSTALL`, and the macOS CI workflow.

## 2. Configure an arm64 development build

```shell
cmake \
  -S . \
  -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_PREFIX_PATH="/opt/homebrew;/opt/homebrew/opt/openssl@3;/opt/homebrew/opt/zlib" \
  -DTESTING=ON \
  -DVERBOSE_CONFIGURE=ON
```

Review the configure summary and confirm that it finds Qt Core, Network, SQL, XML, LinguistTools, GUI, Widgets, SVG, libtorrent 2.1, Boost, OpenSSL 3, and zlib.

## 3. Compile and test

```shell
cmake --build build
cmake --build build --target check
build/qbittorrent.app/Contents/MacOS/qbittorrent -v
```

Then launch the GUI:

```shell
open build/qbittorrent.app
```

## 4. Create a locally distributable app

Bundle Qt and other required libraries:

```shell
macdeployqt build/qbittorrent.app -no-strip
```

Apply an ad-hoc signature with Hardened Runtime for local testing:

```shell
xattr -cr build/qbittorrent.app
codesign --force --deep --options runtime --timestamp=none --sign - build/qbittorrent.app
codesign --verify --deep --strict --verbose build/qbittorrent.app
```

Create the disk image:

```shell
hdiutil create \
  -fs HFS+ \
  -srcfolder build/qbittorrent.app \
  -volname qBittorrent \
  build/qbittorrent.dmg
```

## 5. Final verification

Test on a clean macOS account or another Apple silicon Mac:

- Open the DMG and copy the app to Applications.
- Ensure the app starts without Homebrew paths being required.
- Open a `.torrent` file and a `magnet:` URL.
- Check Web UI startup and the Python-based search feature.
- Inspect bundled dependencies with `otool -L`.

For distribution to other users, manually dispatch the macOS CI workflow with `sign_release` enabled. The trusted release path signs nested code and the app with Hardened Runtime and a secure timestamp, signs and submits the DMG to Apple’s notary service, staples the accepted ticket, and verifies the result with Gatekeeper.

Configure these GitHub Actions secrets before using the release path:

- `MACOS_CERTIFICATE_P12`: base64-encoded Developer ID Application certificate and private key in PKCS #12 format.
- `MACOS_CERTIFICATE_PASSWORD`: password protecting the PKCS #12 file.
- `MACOS_SIGNING_IDENTITY`: full Developer ID Application identity reported by `security find-identity`.
- `APPLE_NOTARY_KEY`: base64-encoded App Store Connect API private key (`.p8`).
- `APPLE_NOTARY_KEY_ID`: App Store Connect API key identifier.
- `APPLE_NOTARY_ISSUER_ID`: App Store Connect API issuer identifier.

Normal push and pull-request builds never import these credentials and continue to produce ad-hoc-signed CI artifacts. Signing material is installed in a temporary keychain and removed even when the job fails.
