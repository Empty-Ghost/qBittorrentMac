# macOS release dependency manifest

Updated: 2026-07-18

This manifest records the selected macOS release inputs. Minimum versions in `CMakeLists.txt` are compatibility checks, not release build pins. Release automation must verify the hashes below before using source archives.

| Component | Selected version | Role | Source and SHA-256 | License | Support policy | Architectures |
| --- | --- | --- | --- | --- | --- | --- |
| Qt | 6.11.1 | Bundled GUI/runtime frameworks | Five universal macOS archives from Qt's online repository; exact filenames and SHA-256 values are recorded in `.github/workflows/helper/qt-6.11.1-macos.sha256` and verified after download | LGPL-3.0/GPL-3.0/commercial | Latest public stable; no public LTS patch stream | Universal `arm64`, `x86_64` frameworks |
| OpenSSL | 3.5.7 | Linked through libtorrent; TLS runtime | `https://github.com/openssl/openssl/releases/download/openssl-3.5.7/openssl-3.5.7.tar.gz`; `a8c0d28a529ca480f9f36cf5792e2cd21984552a3c8e4aa11a24aa31aeac98e8` | Apache-2.0 | LTS through 2030-04-08 | `arm64`, `x86_64` |
| libtorrent | 2.1.0 | Linked BitTorrent engine | `https://github.com/arvidn/libtorrent/releases/download/v2.1.0/libtorrent-rasterbar-2.1.0.tar.gz`; `ceed657606b8df453ec5e775326e3c759a2779e1202fa04abe42ed262e7bf0b6` | BSD-3-Clause | Latest stable; no LTS channel | `arm64`, `x86_64` |
| Boost | 1.91.0 | Headers used by libtorrent/qBittorrent | `https://archives.boost.io/release/1.91.0/source/boost_1_91_0.tar.gz`; `5734305f40a76c30f951c9abd409a45a2a19fb546efe4162119250bbe4d3a463` | BSL-1.0 | Latest stable; no LTS channel | Architecture-independent headers |
| zlib | 1.3.2 | Compression library | `https://zlib.net/zlib-1.3.2.tar.gz`; `bb329a0a2cd0274d05519d61c667c062e06990d72e125ee2dfa8de64f0119d16` | Zlib | Latest stable; no LTS channel | `arm64`, `x86_64` |
| CMake | 4.4.0 | Build-only | `https://github.com/Kitware/CMake/releases/download/v4.4.0/cmake-4.4.0.tar.gz`; `65757f442fdd242e27f1728fc26dc0cba4164f7a0791a5c788631c00080369bc` | BSD-3-Clause | Latest stable; no LTS channel | `arm64`, `x86_64` |
| Ninja | 1.13.2 | Build-only | `https://github.com/ninja-build/ninja/releases/download/v1.13.2/ninja-mac.zip`; `c99048673aa765960a99cf10c6ddb9f1fad506099ff0a0e137ad8960a88f321b` | Apache-2.0 | Latest stable; no LTS channel | Universal `arm64`, `x86_64` binary |
| ccache | 4.13.6 | Build-only compiler cache | `https://github.com/ccache/ccache/releases/download/v4.13.6/ccache-4.13.6-darwin.tar.gz`; `0274210ec9c9936ed5711d59b0de3167a51216a588ddde35f6bc828f366fe6d9` | GPL-3.0-or-later | Latest stable; no LTS channel | Universal `arm64`, `x86_64` binary |
| Syft | 1.48.0 | Packaging-only SBOM generator | `syft_1.48.0_darwin_amd64.tar.gz`: `dc7b2135fa5591003596df4ddb3408f499b68174f5e7dc1c77a373b753463182`; `syft_1.48.0_darwin_arm64.tar.gz`: `fef3e6d5df336a0a4c3e421e503119d1e221cf82a3ef5e426a791fcd81667e87`; both from `https://github.com/anchore/syft/releases/tag/v1.48.0` | Apache-2.0 | Latest stable; no LTS channel | Native `arm64` and `x86_64` archives |
| aqtinstall | 3.3.0 | Build-only Qt installer | Exact Python package version selected by the immutable Qt installer action; each downloaded Qt archive is independently verified against the committed SHA-256 manifest | MIT | Latest stable; no LTS channel | Runs on the selected macOS CI architecture |
| Node.js | 24.18.0 | Web UI tooling only | `https://nodejs.org/dist/v24.18.0/node-v24.18.0.tar.xz`; `e94afde24db08e0c564ee7110a2d5aab51ee0059382c9fd8233c54eec47b28f9` | MIT | LTS line | `arm64`, `x86_64` |
| npm | 11.12.1 | Web UI package manager | Exact version in `package.json`; package dependency integrity is recorded in `package-lock.json` | Artistic-2.0 | Update with Node tooling after Web UI validation | `arm64`, `x86_64` |
| Python | 3.14.6 | Search/developer tooling | `https://www.python.org/ftp/python/3.14.6/Python-3.14.6.tgz`; `74d0d71d0600e477651a077101d6e62d1e2e69b8e992ba18c993dd643b7ba222` | Python-2.0 | Latest stable; runtime floor remains separately declared | `arm64`, `x86_64` |
| uv | 0.11.29 | Python environment manager | Exact release selected by the pinned setup action; package integrity is recorded in `uv.lock` | Apache-2.0/MIT | Latest stable; no LTS channel | `arm64`, `x86_64` |

## Update procedure

- The build/release maintainer owns direct dependencies and verifies versions, source URLs, hashes, licenses, and upstream support status together.
- Dependabot proposes npm, Python, and GitHub Actions updates. A human reviews and submits all upstream changes as required by this repository's contribution policy.
- Update one dependency family at a time, regenerate only its lock data, and run the native, Web UI, and search-tool checks that apply.
- For vendored Lua, LuaBridge, and expected-lite, record upstream provenance and local patches here before changing excluded vendor paths.
