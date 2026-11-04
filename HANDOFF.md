# island-simulator — current maintainer handoff

Reviewed: 2026-11-04. This file summarizes the maintenance state; chronology belongs in Git history.

## Current state

- The project is a C++23 application built on CNA. The working branch is `develop`.
- Latest production-code changes: `52d8c56e` (octopus seabed contact),
  `05077820` (reef-boulder clearance), `5e74c5d0` (shared lighting for octopuses and stingrays).
- The latest Web build was produced from source revision `30106358` on 2026-11-03.
  That commit also reverted the mistakenly added `AGENTS.md` and `CLAUDE.md`; no new AI policy was adopted.
- The visual campaign has recorded native software-renderer validation. This does not qualify every platform.
  Failed Web checks and unresolved issues are explicitly listed below.
- Old instructions to continue indefinitely and historical hour targets are not current work assignments.

## Code map

| Area | Main entry points |
| --- | --- |
| Game loop, cameras, system integration | [IslandGame.cpp](game/src/IslandGame.cpp) |
| Terrain height and the actual rendered surface | [IslandTerrain.cpp](game/src/IslandTerrain.cpp), [RenderedTerrainSurface.cpp](game/src/RenderedTerrainSurface.cpp) |
| Moisture, exposure and coastal environment | [IslandEnvironmentField.hpp](game/include/IslandSimulator/IslandEnvironmentField.hpp) |
| Vegetation and natural-object placement | [JungleLayout.cpp](game/src/JungleLayout.cpp), [GrassLayout.cpp](game/src/GrassLayout.cpp), [GroundCoverField.cpp](game/src/GroundCoverField.cpp), [NaturalPropLayout.cpp](game/src/NaturalPropLayout.cpp) |
| Vegetation, ocean, shadows | [JungleRenderer.cpp](game/src/JungleRenderer.cpp), [OceanRenderer.cpp](game/src/OceanRenderer.cpp), [SunShadowMap.cpp](game/src/SunShadowMap.cpp) |
| Weather and time of day | [WeatherSystem.cpp](game/src/WeatherSystem.cpp), [TimeOfDayRenderer.cpp](game/src/TimeOfDayRenderer.cpp), [SkyRenderer.cpp](game/src/SkyRenderer.cpp) |
| Walking and vehicle traversal | [PlayerMotion.cpp](game/src/PlayerMotion.cpp), [QuadBike.cpp](game/src/QuadBike.cpp) |
| Wildlife and resident | Groups/herds and their `*Renderer.cpp` files; e.g. [OctopusGroup.cpp](game/src/OctopusGroup.cpp), [ResidentMotion.cpp](game/src/ResidentMotion.cpp) |

Headers are in `game/include/IslandSimulator/`, tests in `tests/`, build helpers and validators in `tools/`.
See [architecture](docs/architecture.md) for more detail; changing one system does not require reading the entire feature inventory.

## Safety and dependencies

- Compile on at most **four CPU cores**. Use `taskset --cpu-list 0,1,2,3` and `-j2`;
  configure-time dependency builds and WebAssembly optimization must inherit the same CPU limit.
- **The AMDGPU GPUVM incident remains open.** Run automated graphics checks through the existing
  isolated software launchers. Verify the actual renderer; an environment variable alone is insufficient.
  Procedure and limits: [incident report](docs/incidents/2026-10-08-amdgpu-gpuvm-reset.md).
- External CNA and Sharp Runtime are read-only. Do not modify their worktrees or system drivers.
  [dependencies.lock](dependencies.lock) defines the pinned revisions, not the current upstream `next` branch.
- Preserve other worktree changes; do not use reset/clean or rewrite remote history.

## Build and run on this machine

The existing `build/` is **Release/OPENGL33**; `build-probe/` is **Release/WEBGL2**.
For a new installation, follow [Linux](docs/build-linux.md) or [Web](docs/build-web.md);
these commands reuse the already configured directories.

Native build and tests without a graphics device:

```sh
export CCACHE_DIR=/rv/cnaccache CCACHE_BASEDIR=/rv
taskset --cpu-list 0,1,2,3 cmake --build build -j2
taskset --cpu-list 0,1,2,3 ctest --test-dir build -LE display -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2
python3 tests/SoftwareCaptureTests.py
```

The two excluded validators require separately generated input artifacts; these CPU checks do not replace graphics validation.
Safe native capture: `python3 tools/run_software_capture.py build jungle`.
The launcher requires kernel-journal access and verifies containment and the software renderer.

Web build, packaging and local HTTP server:

```sh
source /home/robertvokac/emsdk/emsdk_env.sh
export CCACHE_DIR=/rv/cnaccache CCACHE_BASEDIR=/rv
export EMCC_CORES=2 BINARYEN_CORES=2 CMAKE_BUILD_PARALLEL_LEVEL=2
taskset --cpu-list 0,1,2,3 cmake --build --preset web-webgl2
tools/package_web_release.sh build-probe
python3 -m http.server 8765 --bind 127.0.0.1 --directory build-probe/web-release
```

Open `http://127.0.0.1:8765/island-simulator.html`; do not launch the files through `file://`.
Latest ZIP: `build-probe/island-simulator-web-30106358.zip` (14,473,341 bytes).
The package contains HTML, JS, WASM, data, gzip variants and a hash manifest.

## Last recorded validation — 2026-11-03

| Check | Result and limits |
| --- | --- |
| Linux Release/OPENGL33 | 109/109 safe CTests and 79 launcher tests passed. Display tests and the two standalone validators are excluded from that count. |
| Native visuals | 36 current software-rendered scenes / 80 PNGs; 36 main images and 28 additional stages reviewed, 16 stages byte-identical to previously reviewed images. llvmpipe/Pixman, not physical-GPU validation. |
| Web build | Release/WEBGL2 compilation, asset validation and ZIP/gzip integrity passed. |
| Chrome / SwiftShader | Island overview rendered; 60 frame intervals, GL error 0, context preserved. The complete smoke test **failed** on a stale expected tree-geometry count. |
| Firefox / llvmpipe | Production page, controls, resizing and tab-return checks completed before the population check. The complete smoke test **failed** on stale vegetation totals. Three production-page images were visually reviewed. |
| Other platforms | No new physical-GPU, Android, Debug or other-backend qualification is implied. Older results retain their dates in the platform documents. |

Editing this handoff does not add new build or rendering results.

## Open issues and follow-up work

1. **Web smoke tests contain stale constants.** Chrome expects 4998/4864 near-mesh vertices/triangles
   and 1071/1120 far-mesh vertices/triangles; current native and Web builds both produce
   5010/4888 and 1076/1130. Firefox expects population totals 261/1747/569/3087;
   current totals are 255/1637/543/2952. Check the generators and rerun the relevant test when updating these;
   do not relabel retained failed runs as passes. Scripts: `tools/run_web_smoke.mjs`
   and `tools/run_firefox_web_smoke.mjs`.
2. **CNA-ISLAND-002:** confirmed stock-skinning shader defect on the tested GLES uniform limit.
   The game-side monkey workaround remains active. The 2026-11-03 upstream audit is not a new Android test.
3. **CNA-ISLAND-001:** suspected Android surface/Home-resume race; a fix has not been verified.
   Both findings and their closure requirements are in [BUGS.md](BUGS.md).
4. **AMDGPU/flight instability:** cause and hardware qualification remain open.
   Software captures do not close these incidents.
5. Procedural-motion limitations, soft-vegetation intersections and reverse quad traction on an
   approximately 18-degree ramp remain. See [known limitations](docs/known-limitations.md).
6. Historical whole-process timings vary with host load/cache/JIT. The controlled software
   octopus contact/shadow experiment measured mean process CPU +1.10% and approximately
   +0.156 ms for shadow submission; this is neither a universal bound nor a demonstrated whole-game speedup.

## Evidence and history — consult as needed

- Native verification and delivery: `build/campaign-contact-final-verification.json`,
  `build/campaign-contact-final-delivery.json`.
- Latest Web build and both failed smoke runs: `build/web-build-20261103-result.json`.
- Protection of previously cited captures against automatic pruning:
  `build/handoff-evidence-retention.json`. This is a machine index, not another reading assignment.
- Files under `build*/` are local ignored artifacts; a clean Git checkout does not contain them.
- Full previous diaries remain in history, without another copy in the live documentation:
  `git show 30106358:handoff.md` and `git show 30106358:HANDOFF.md`.
- [README.md](README.md) is the project entry point, [BUGS.md](BUGS.md) records framework issues,
  [ASSET_LICENSES.md](ASSET_LICENSES.md) records asset provenance. [PLAN.md](PLAN.md) also contains
  extensive historical work logs; do not treat old deadlines, states or instructions as today's assignment.
