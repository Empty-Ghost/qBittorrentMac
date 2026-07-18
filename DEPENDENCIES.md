# macOS release dependency manifest

Updated: 2026-07-18

This manifest records the selected macOS release inputs. Minimum versions in `CMakeLists.txt` are compatibility checks, not release build pins. Release automation must verify the hashes below before using source archives.

| Component | Selected version | Role | Source and SHA-256 | License | Support policy | Architectures |
| --- | --- | --- | --- | --- | --- | --- |
| Qt | 6.11.1 | Bundled GUI/runtime frameworks | Qt online installer archives, selected by exact version in macOS CI; per-archive hash capture is pending | LGPL-3.0/GPL-3.0/commercial | Latest public stable; no public LTS patch stream | `arm64`, `x86_64` |
| OpenSSL | 3.5.7 | Linked through libtorrent; TLS runtime | `https://github.com/openssl/openssl/releases/download/openssl-3.5.7/openssl-3.5.7.tar.gz`; `a8c0d28a529ca480f9f36cf5792e2cd21984552a3c8e4aa11a24aa31aeac98e8` | Apache-2.0 | LTS through 2030-04-08 | `arm64`, `x86_64` |
| libtorrent | 2.1.0 | Linked BitTorrent engine | `https://github.com/arvidn/libtorrent/releases/download/v2.1.0/libtorrent-rasterbar-2.1.0.tar.gz`; `ceed657606b8df453ec5e775326e3c759a2779e1202fa04abe42ed262e7bf0b6` | BSD-3-Clause | Latest stable; no LTS channel | `arm64`, `x86_64` |
| Boost | 1.91.0 | Headers used by libtorrent/qBittorrent | `https://archives.boost.io/release/1.91.0/source/boost_1_91_0.tar.gz`; `5734305f40a76c30f951c9abd409a45a2a19fb546efe4162119250bbe4d3a463` | BSL-1.0 | Latest stable; no LTS channel | Architecture-independent headers |
| zlib | 1.3.2 | Compression library | `https://zlib.net/zlib-1.3.2.tar.gz`; `bb329a0a2cd0274d05519d61c667c062e06990d72e125ee2dfa8de64f0119d16` | Zlib | Latest stable; no LTS channel | `arm64`, `x86_64` |
| CMake | 4.4.0 | Build-only | `https://github.com/Kitware/CMake/releases/download/v4.4.0/cmake-4.4.0.tar.gz`; `65757f442fdd242e27f1728fc26dc0cba4164f7a0791a5c788631c00080369bc` | BSD-3-Clause | Latest stable; no LTS channel | `arm64`, `x86_64` |
| Node.js | 24.18.0 | Web UI tooling only | `https://nodejs.org/dist/v24.18.0/node-v24.18.0.tar.xz`; `e94afde24db08e0c564ee7110a2d5aab51ee0059382c9fd8233c54eec47b28f9` | MIT | LTS line | `arm64`, `x86_64` |
| npm | 11.12.1 | Web UI package manager | Exact version in `package.json`; package dependency integrity is recorded in `package-lock.json` | Artistic-2.0 | Update with Node tooling after Web UI validation | `arm64`, `x86_64` |
| Python | 3.14.6 | Search/developer tooling | `https://www.python.org/ftp/python/3.14.6/Python-3.14.6.tgz`; `74d0d71d0600e477651a077101d6e62d1e2e69b8e992ba18c993dd643b7ba222` | Python-2.0 | Latest stable; runtime floor remains separately declared | `arm64`, `x86_64` |
| uv | 0.11.29 | Python environment manager | Exact release selected by the pinned setup action; package integrity is recorded in `uv.lock` | Apache-2.0/MIT | Latest stable; no LTS channel | `arm64`, `x86_64` |

## Update procedure

- The build/release maintainer owns direct dependencies and verifies versions, source URLs, hashes, licenses, and upstream support status together.
- Dependabot proposes npm, Python, and GitHub Actions updates. A human reviews and submits all upstream changes as required by this repository's contribution policy.
- Update one dependency family at a time, regenerate only its lock data, and run the native, Web UI, and search-tool checks that apply.
- For vendored Lua, LuaBridge, and expected-lite, record upstream provenance and local patches here before changing excluded vendor paths.

## Known manifest gaps

- Capture and verify each Qt online-installer archive hash rather than relying only on the install action's cache behavior.
- Add exact Ninja and ccache versions and hashes when CI stops relying on runner-provided copies.
- Generate a CycloneDX SBOM from the final application bundle and retain it with release artifacts.
