# Comprehensive visual cohesion campaign — ACTIVE continuation afterd50ccc9f

The whole original superprompt/attachment is assessed in the final acceptance ledger at the
end of this file. Earlier premature completion conclusions and ACTIVE checkpoints remain as
history, not the current result. The final record includes the user-identified alien/UFO/castle
repairs, real bunker turnaround/camera corrections, a fresh holistic matrix and live scenarios.
It explicitly separates available software qualification from still-open hardware/platform limits.

Entry develop/8b84844e77e34ac54d4259f397079ff72ec34634,63tracked modifications+19new files,
all preserved and committed/pushed in ad48d3d4 at the user's explicit request. Entry app66234fb26194a6c1b603cf6a756e1fcf7c6ae53370234504d4de6f8bbb4252c9
saved in build/campaign-deer-alert-entry-binary. Builds used CPU0–3/-j2/-j3; physical AMDGPU suspended,
contained verified llvmpipe/Pixman only. No external source/system/Git destructive operations.

Continuation entry2026-11-03: develop/88a20f5a clean/synced, earlier work/evidence preserved.
The goal was reopened and the first observed remaining priority was stance slip:
measure fixed-world elephant contacts during acceleration and actual Watch turning, capture
matched baseline through existing software QA, then implement bounded foot anchoring using the
existing rig. Next reassess other observed locomotion/environment defects, continue stages and
fresh full original acceptance. Max4compilation cores; contained verified llvmpipe/Pixman only.
Sound remains closed. No external dependency source changes or destructive Git operations.

**No temporary diagnostic, disabled aerial range or benchmark-only source remains.**
Final source enables the qualified aerial canopy and palm policies plus the final resident waist
correction. Benchmark pairs isolate their respective changes; the palm benchmark binaries precede
the final waist correction. Their benchmark run.json files contain launch-time binary identities;
final resident captures used the final game binary without intervening rebuild/source mutation.
`build/campaign-continuation-final-identity.json` records its hash AFTER those runs (the capture
harness did not yet record launch-time executable hashes at that checkpoint).
No pending build/test/render process. Latest continuation and full acceptance evidence are in the last sections.
Further polish should start from the remaining limits below, without reopening rejected hypotheses
or recreating existing ecology, landmarks, species, animation or rendering architecture.


## Historical acceptance checkpoint — invalidated by later defects (2026-11-02)

This earlier completion assessment was premature: subsequent user inspection found unfinished
UFO, alien and castle presentation, and continued traversal checks found missing bunker divider
collision. Retain the table as historical evidence only. It does not supersede ACTIVE status or
the original full-campaign criteria. The latest qualified changes and pending work are below.

| Priority | Final evidence |
| --- | --- |
| Forest and vegetation | Correlated ecology retained; floor pigment/root grounding/aerial canopy and palm corrections; forest-review, root-contact and canopy-LOD comparisons |
| Terrain and geology | Rock dual-chart material, grounded props, mine approach and cave support; six matched geology views plus near/coastal/elevated review |
| Water and coastal integration | Caustic/foam corrections; seven-view coast/reef review; final tide-relative live diving |
| Wildlife and resident | Bear face, boar/deer gait/support, turtle nesting, resident sleeve/normals/waist; live motion stages and static controls |
| Lighting, shadows and weather | Existing SunShadowMap aerial focus, shelter/drips, wet organic contrast, covered-cave rain; clear/dawn/cloud/rain/night/interior matrix |
| Landmarks and traversal | All landmarks retained/reviewed; mine/bunker grounding, stream/cave walking, swimming/flight; real bunker entry/exit regression and strict functional gate |
| Performance and maintainability | Controlled software pairs/variance investigations, unchanged world fingerprint, focused helpers/tests, no external patches |
| Final qualification | Final exact-build holistic and live-input images inspected; 83/83 safe CTests,70/70 harness,11/11 validator; audit and repository state recorded |

That review missed real defects later recorded below. Its completion conclusion is withdrawn.
Continue observed corrections without repeating risky hardware experiments or inventing features.
Unverified hardware/platform behavior and explicit simulation bounds are not represented as passed.

## Implemented and reviewed

- Moss suitability shares CPU/GLSL moisture/canopy-transmission response. Terrain reuses existing
  environmental samples; shaded grass receives restrained pigment. Added endpoint, monotonicity
  and invalid-input coverage. Existing terrain heightfield, placements and collision remain intact.
- Mangrove stilt feet follow exact bank height at low vertices, using a private helper in visible
  and sun-shadow shaders. Conservative bounds include the lowered supports. No X/Z/layout change.
- Aerial canopy smoothly restores the existing full-profile reach above the crowns, bounded by
  projected silhouette size. Ground reach, understory and near/medium meshes remain unchanged.
  Smaller juveniles keep their original budget. Clearance is above terrain OR sea surface, so
  deep-water swimming is not mistaken for altitude. Stock/noninstanced fallback retains its range.
- Extra aerial trees use cheaper index ranges in existing buffers: same trunk/crown anchors and
  upper/perimeter leaves, simplified unresolved faces/folds. Ordinary variants use 200 rather
  than 1,120 triangles; mangroves 340 rather than 1,260. Shared instance matrices upload once,
  then standard binding offsets draw both ranges, respecting XNA's bound-buffer update rules.
  Extra indices: 11,640 bytes. Static vertex payload unchanged: 3,543,852 bytes, 52-byte vertices,
  nine attributes. No extra vertex buffers/textures. Overview shows 192 existing canopies instead
  of 42; bay shows 231. Total generated canopy population remains 255.
- Seabed caustic brightness varies through existing warp components, reducing the continuous grid
  without extra noise/geometry/resources. Foam pigment follows its share of composed opacity,
  preserving wave/foam fields and alpha while making clear-water disturbances read as pale foam.
- Bear brow ridges embedded/shrunk; original UVs, atlas and motion retained. Resident has one
  coherent open sleeve and taper-aware torso/neck normals. Same rig, routes and two draws;
  near body drops from 7,142 to 6,814 triangles (−4.6%).
- Walking acceleration/contact integrates in bounded <=1/60 s steps inside the original 100 ms
  clamp. Idle/swimming retain one-step paths; full-frame trace counters aggregate. Original
  slope/step/obstacle thresholds remain. Speed-aware downhill follow fixes the observed 9 cm hover.
  Bidirectional 60 Hz/100 ms stream regression was RED on the old bank-locking controller.
- Cave sweep tests actual supported feet instead of a proxy raised 1 mm. New entry/back-wall
  whole-capsule assertions reproduced penetration and pass after correction. Strict live gate kept.
- Cave QA tests its back wall, then roof/floor in the open chamber. Diving telemetry/QA uses actual
  tide-relative height/depth, with tidal/corrupt fixtures. General named screenshots now advance
  once per frame on the existing explicit clock pattern; inspection retains its frozen clock.
  Harness rejects missing/conflicting/catch-up accounting. Two repeated water PNGs are byte-identical.

## Build, safety and compilation constraint

**User requires compilation on at most four CPU cores.** All future compilation must retain
explicit affinity to at most four permitted logical CPUs and at most three jobs. Current allowed
set includes 0–3; last builds used:

```sh
taskset --cpu-list 0,1,2,3 env CCACHE_DIR="$PWD/build/ccache" cmake --build build -j3
```

Build configured Release/OpenGL33, Ninja, GCC 14.2, tests/tools enabled. Shared dependency trees
had advanced; exact locked CNA archive and pinned Sharp Runtime/EasyGL/MetaGL clones are under
ignored `build/dependencies`. Shared sources remained read-only; no CNA/Sharp Runtime patch.
CNA archive lacks submodule data; no Web/Android build was attempted. No AGENTS.md found in repo
or ancestors. CoreTests.cpp now has only the focused rock bounds assertion update; IslandGame.cpp receives
only narrow integration/QA edits.

AMDGPU GPUVM incident remains OPEN. Read README, BUGS, PLAN and incident docs before more graphics.
Never exercise the affected hardware, reproduce the incident, clear its stop latch or change
kernel/driver/system configuration. Safe paths used:

```sh
python3 tools/run_software_capture.py build <view>
python3 tools/run_fullscreen_software.py build --cave   # also --stream-crossing, --diving, --flight
```

Require sandbox exception for private compositor sockets. Existing supervisor verifies runtime
llvmpipe and Weston Pixman, private bwrap devices/PID lifetime, kernel-journal access/monitoring.
Never bypass containment or renderer verification. This is native application testing on a
SOFTWARE renderer, not physical/native-GPU qualification. All qualified kernel fault deltas are empty.

## Final validation

```sh
ctest --test-dir build -LE display -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2
python3 tests/SoftwareCaptureTests.py
build/island-world-report --output build/campaign-world-final.json
```

- Final C++ suite: **76/76**, 90.03 s, `build/campaign-final-cpu.log`.
- Capture harness: **59/59**, `build/campaign-final-python.log`.
- The 36 display-tagged CTest entries were not run as configured. Two additional report
  validators lack their excluded display-producer inputs. Graphical qualification
  instead used the supervised software paths; no orphan-validator success is claimed.
- World report parsed identical to the pre-final report; fingerprint `238be1fec0a0cdd7`.
- Twenty-view holistic audit passed and contact/full representative images were reviewed:
  night water, overcast dawn jungle, rainy hut, island/clearing/ridge/beach/bay/cave roof,
  lighthouse/mine/bunker/wreck, crocodiles/birds/fish/turtles/monkeys/goats/boars.
- Final restored-source aerial/walking/quad/LOD captures pass. Walking jungle/water PNGs remain
  byte-identical to their pre-aerial accepted counterparts. Quad route renders actual driving;
  all 22 landmark footprint audit rows report zero intersections.
- Canonical ordinary/mangrove near/far pairs inspected: changes stay within selected crown regions
  (63×74 and 27×42 pixel bounds), preserving broad silhouette. The first ad-hoc target was occluded
  or held near by grip constraints and had identical pixels; it is not evidence of a visible switch.
- Strict live cave, stream crossing, diving/audio/resurfacing and final flight-ceiling/landing pass.
  Flight crosses the new altitude policy under real keyboard input; ceiling/landing images reviewed.
- Release final build and `git diff --check` pass. No Debug, Web, Android, stock-fallback visual or
  physical-GPU qualification. Existing external issues in BUGS.md remain unchanged.

## Accepted visual evidence

Use `build/campaign-final-comparisons/`: `aerial.png`, `contact.png`, `resident.png`,
`canopy-lod.png`, `holistic.png`, plus JSON indexes. General old captures had wall-time animation
variation; the synchronized comparisons below use identical camera/seed/resolution/lighting and
explicit clock. Original renderer sources were temporarily built for baselines, then restored.
All following screenshot directories are under `build/software-captures/`:

- jungle: `20261101T202458.419218Z-jungle` → `20261101T202654.255644Z-jungle`.
- stream: `20261101T202506.349956Z-stream` → `20261101T202701.758573Z-stream`.
- mangroves: `20261101T202516.615257Z-mangroves` → `20261101T202712.289368Z-mangroves`.
- bear: `20261101T202524.290687Z-bear` → `20261101T202720.024170Z-bear`.
- reef-habitat: `20261101T202538.582418Z-reef-habitat` → `20261101T202735.431356Z-reef-habitat`.
- water: `20261101T202545.646013Z-water` → `20261101T202742.718507Z-water`.

Final aerial/controls/vehicle/initial inspection captures:
- `20261101T214439.073709Z-overview`
- `20261101T214449.598107Z-bay`
- `20261101T214459.903033Z-jungle`
- `20261101T214510.183869Z-water`
- `20261101T214520.476036Z-quad-drive`
- `20261101T214536.471961Z-quad-audit`
- `20261101T214547.316884Z-canopy`
- `20261101T214557.572133Z-canopy`

Canonical LOD inspection (ordinary near/far, mangrove near/far):
- `20261101T215158.017721Z-canopy`
- `20261101T215205.522750Z-canopy`
- `20261101T215212.966933Z-canopy`
- `20261101T215220.414435Z-canopy`

Resident before → after:
- `20261101T202047.567182Z-resident` → `20261101T203209.981504Z-resident`
- `20261101T202102.325500Z-resident-tending` → `20261101T203223.815313Z-resident-tending`
- `20261101T202110.674413Z-resident-fishing` → `20261101T203231.125088Z-resident-fishing`

Live evidence:
- `build/software-cave/20261101T195929.238812Z`: 76 samples, zero solid capsules.
- `build/software-stream-crossing/20261101T200047.411736Z`: support error <=4.77e−7 m.
- `build/software-fullscreen/20261101T200123.012685Z`: tidal dive, audio, resurface.
- `build/software-flight/20261101T215548.405153Z`: ceiling, held ascent, landing and controls.

Holistic capture directories (reviewed contact sheet; retain for provenance):
- `20261101T203559.469065Z-water-t19.50`
- `20261101T203608.164720Z-jungle-t06.20-overcast`
- `20261101T203616.884048Z-hut-showers`
- `20261101T203625.638648Z-overview`
- `20261101T203633.907308Z-clearing-edge`
- `20261101T203642.432546Z-west`
- `20261101T203650.977702Z-beach`
- `20261101T203659.929160Z-bay`
- `20261101T203708.451037Z-cave-top`
- `20261101T203716.635081Z-lighthouse`
- `20261101T203723.993547Z-mine`
- `20261101T203731.794355Z-bunker`
- `20261101T203739.301943Z-shipwreck`
- `20261101T203746.366142Z-crocodiles`
- `20261101T203753.850394Z-birds`
- `20261101T203801.358683Z-fish`
- `20261101T203808.631259Z-turtles`
- `20261101T203816.122321Z-monkeys`
- `20261101T203826.949976Z-goats`
- `20261101T203840.998900Z-boars`

## Performance evidence and limits

Same 1280×720, VSync off, llvmpipe eight threads, 120 warmup +600 measured intervals. Existing
safe supervisor reused by ignored `build/campaign_software_benchmark.py`. No simultaneous campaign
builds/tests/graphics during timing. Reports under `build/campaign-performance/`.

| Comparison | Mean ms before → after | Notes |
|---|---:|---|
| Reef caustics | 71.2557 → 72.5685 (+1.84%) | Terrain 9 cells/7 draws unchanged |
| Foam repeat | 132.3998 → 140.6417 (+6.2%) | Same 143–146 tracked submissions |
| Final aerial OFF/ON pair | 104.1945 → 110.9105 (+6.45%) | 42 →192 canopies; 56–57 →65–66 tracked draws |

Final pair uses identical final renderer binaries differing only in aerial range, run back-to-back
with the same settings (`canopy-{off,on}-controlled`). Actual game CPU user+system includes startup/
warmup: 451.74 →482.95 s (+6.91%); peak process RSS 784,524 →799,880 KiB (+1.96%). p95 frame
interval 115.0864 →122.7525 ms (+6.66%). Direct game `/usr/bin/time` reports are inside each set.
Outer launcher-only time reports are INVALID as game CPU/RSS and must not be cited.

Earlier aerial wall timings varied 74.8–123.5 ms; read-only inspection confirmed unrelated CPU-heavy
applications. No other processes were changed. All reports retained; do not conceal variance or
claim native FPS. Raw all-far expansion was expensive; core-only trees looked round. Final index
ranges preserve useful outlines at 200/340 triangles and one matrix upload per variant. Original
walk budgets and final PNG controls unchanged. Root/idle benchmarks varied too; no speedup claim.
Native hardware performance and GPU memory remain unmeasured. Peak RSS above is software-process
memory, not GPU VRAM. First-use four-frame submission timings include shader JIT, not sustained cost.

## Remaining limitations / rejected work

- Hardware incident unresolved; all physical GPU, alternate backend and platform qualification
  remains open. Root grounding/aerial policy are qualified on custom instanced OpenGL33 only;
  stock fallback preserves its existing behavior.
- Earlier forehead UV work was rejected. The later face-loft-only flow correction below is
  accepted after four matched day/wet/dawn comparisons; body UVs and coarse128px coat remain.
  Do not deliver rejected cropped-material or older diagnostic images as accepted.
- Night resident walking gate expects daytime locomotion; a seated evening pose failed that
  fixture. This is not a reproduced resident movement bug or nighttime walking qualification.
  The supported evening tending pose is now visually qualified in the continuation below.
- Suspected foreground water facets were intentional fixed-view player rings. Constant-output,
  no-Fresnel and no-player-ring diagnostics isolated this. Fine-depth/mean-absorption/shallow-wave/
  flat-geometry experiments did not explain it and were removed. No CNA defect established.
- Highest-support-only player slope experiment rejected; original slope semantics retained.
- A bound instance-stream update was caught during new LOD development; final code uploads once
  and uses binding offsets. CNA's guard was correct; external sources unchanged.
- This is a bounded polish campaign, not a claim of zero defects or photorealism. Existing terrain,
  ecology, animation and landmark systems were retained where audits showed no in-scope defect.


## Continuation — aerial palms and resident waist (qualified, 2026-11-02)

- Fixed observed missing coastal silhouettes above the crowns. Living palms now share the
  canopy's gradual altitude/projected-size policy, bounded by their own existing full 520 m
  profile. Original 286 m walking reach, low broken/fallen props and stock fallback remain.
  One existing terrain/sea clearance sample serves jungle and palms. No placement changes.
- Reuses existing far meshes, shaders and dynamic matrix streams; instance capacity can grow
  under the existing doubling policy. Overview retains 93 →179 palms with the same 14 palm
  draws; bay 47 →213 with the same 4. Three walking controls are byte-identical PNGs.
- Evening inspection exposed an oversized circular belt around an oval torso. The band now
  follows the waist and the wrap's upper corners tuck inside it. Original lower corners still
  follow the thighs; shorts, motion and atlas are retained. Same 6,814 body triangles, 24 shadow
  triangles and 2 draws. First belt-only candidate exposed protruding wrap corners; a temporary
  no-shorts diagnostic identified the panels, not the shorts. Diagnostic removed, shorts restored.
- Extended the existing bounded/monotone/projected-size regression to both 475 m broadleaf and
  520 m palm profiles. Builds retain taskset 0–3 and -j3; Release/OpenGL33 build passes.
- Final C++ suite **76/76**, 127.07 s, `build/campaign-continuation-qualified-cpu.log`.
  This run overlapped software captures; its duration is not a performance comparison.
  Python **59/59**, `build/campaign-palm-final-python.log`. World report
  `build/campaign-palm-world-final.json` is parsed-identical, fingerprint `238be1fec0a0cdd7`.
  Same display-test exclusions and hardware/platform qualification gaps as above.
- All five palm capture gates and four final resident gates pass on verified llvmpipe/Pixman.
  Actual full images and both comparison sheets reviewed. Resident eight-stage gait gate passes;
  changes stay within the waist/garment image bounds. Evening tending covers presentation,
  without weakening the daytime walking fixture.
- Live flight passes: `build/software-flight/20261101T225533.286876Z`, including held ceiling
  ascent and safe return. Ceiling/landing screenshots reviewed. `kernel-final.json` is empty;
  palm/resident capture and benchmark kernel deltas are also empty. This live flight tested the
  palm implementation before the final cosmetic waist correction.
- Reviewed sheets: `build/campaign-final-comparisons/palm-aerial.png` and `resident-waist.png`.
  Their JSON indexes retain exact paired source paths. No new landmark/species/assets/dependencies.

### Continuation software performance

Identical overview settings: 1280×720, VSync off, LP_NUM_THREADS=8, 120 warmup intervals +600
samples; same approved private supervisor and direct game `/usr/bin/time -v`. No campaign
build/test/capture overlapped these benchmarks. These isolate palm visibility before the cosmetic
waist correction. Existing tracked draws remain 65–66, canopies 192, terrain 58 cells/6 draws.

| Pair (before →after) | Mean ms | p95 ms | Game CPU user+system s | Peak game RSS KiB |
| --- | --- | --- | --- | --- |
| `palm-{before,after}-controlled` | 92.0785 →96.8316 | 102.8919 →115.4822 | 408.28 →434.10 | 803240 →813420 |
| `palm-{before,after}-reverse-controlled` (after ran first) | 112.3561 →100.5896 | 159.1794 →114.6528 | 442.65 →435.77 | 801416 →809732 |

Raw reports are under `build/campaign-performance/`. First pair mean +5.16%, p95 +12.24%, CPU
+6.32%, RSS +1.27%. The >10% tail change prompted the reverse-order pair: mean -10.47%, p95
-27.97%, CPU -1.55%, RSS +1.04%. The initial warm pair `palm-aerial-{before,after}` was
90.3930 →83.0578 ms. Host/scheduler variance prevents a stable timing delta or speedup claim.
More existing low-detail instances are submitted, with no extra draws/static mesh payload;
process RSS is not GPU VRAM. Physical-GPU cost remains unverified. Resident waist topology,
uploaded vertex count and draw budget are unchanged; no separate resident timing claim.

### Exact continuation capture pairs

All below are under `build/software-captures/`; left is before, right is accepted after.
The five palm pairs precede the additional cosmetic resident correction.

| View | Before UTC directory | After UTC directory |
| --- | --- | --- |
| overview | `20261101T223423.973646Z-overview` | `20261101T224049.096206Z-overview` |
| bay | `20261101T223433.732112Z-bay` | `20261101T224058.030481Z-bay` |
| palms walking | `20261101T223442.273936Z-palms` | `20261101T224106.728891Z-palms` |
| jungle walking | `20261101T223451.234224Z-jungle` | `20261101T224115.871736Z-jungle` |
| water walking | `20261101T223459.748872Z-water` | `20261101T224124.766938Z-water` |
| resident gait | `20261101T225754.734461Z-resident` | `20261101T230353.495410Z-resident` |
| resident tending | `20261101T225816.011769Z-resident-tending` | `20261101T230340.283592Z-resident-tending` |
| resident fishing | `20261101T225828.298312Z-resident-fishing` | `20261101T230420.592285Z-resident-fishing` |
| resident tending 19:30 | `20261101T225634.196978Z-resident-tending-t19.50` | `20261101T230434.969507Z-resident-tending-t19.50` |

Build/capture logs: `build/campaign-palm-final-build.log`, `campaign-palm-{before,after}.log`,
`campaign-resident-belt-final-build.log`, `campaign-resident-belt-final.log`, `campaign-palm-flight.log`.
Original before binaries/source snapshots and rejected intermediate screenshots remain ignored
build evidence, not accepted implementations. No temporary source override remains.

Resident-waist stage binary SHA256: `7b02acdbe2bc400a108368b8761f9adbdc02ce7ddc98707aec0b2a0f044473fc`.


## Continuation — aerial sun-shadow focus (qualified, 2026-11-02)

Observed overview/bay rendered an empty sun-shadow map (0 casters): the camera looked at the
island while the existing 72 m half-extent map stayed over sea. New SunShadowViewFocus keeps
the original receiver column through 12 m clearance, blends above crowns (full at 80 m), uses
the existing 12 m target plane, fades continuously through the horizon and caps horizontal
look-ahead at 280 m. No map extent, resolution, texture, shader-tap or pass expansion.

- SunShadowMap::Prepare receives the focus column. ContactShadowRenderer uses the SAME column
  for CPU/GPU structure-share fading and cache invalidation; actual camera still controls
  distance/frustum culling. The existing terrain-clearance query moved before the shadow pass
  and remains shared with jungle/palms. No new terrain raycast, geometry or procedural placement.
- Added a small independent CPU regression target for exact walking/swimming preservation,
  correct view-column extraction/direction, projected focus coverage, bounded/monotone altitude
  transition, continuous horizon tilt and invalid-direction/clearance fallback. CoreTests.cpp
  remains unchanged. New header/test plus bounded integration; no external sources modified.
- Release/OpenGL33 build passes, taskset 0–3 and -j3. Final C++ **77/77**, **82.80 s**,
  `build/campaign-shadow-focus-final-cpu.log`; Python **59/59**,
  `build/campaign-shadow-focus-final-python.log`. Same 36 display-tagged tests and 2 orphan
  validators excluded, replaced by supervised software qualification, not claimed passed.
- `build/campaign-shadow-focus-world-final.json` is parsed-identical to the previous world report,
  fingerprint `238be1fec0a0cdd7`. Five matching before/after capture gates pass. Overview casters
  0 →1179, bay 0 →1521, flight detail 1468 →979 (coverage moves towards the view), jungle 1620
  unchanged. Walking jungle and --no-sun-shadows overview PNGs are BYTE-IDENTICAL controls.
- Full overview/bay/flight-turn images and both comparison sheets reviewed. Forest shadows now
  ground the viewed stands. No obvious new artifact in reviewed frames; this remains one local
  144 m wide map with its existing fade, not whole-island shadow coverage at every distance.
- Existing live flight QA now yaws right/back at a stationary ceiling eye and records
  flight-turn.png / flight-return.png. Identical script passes with saved before and candidate
  binaries: held Tab, ascent/ceiling, pitch/yaw, descent/opposing keys, safe dry-land return.
  Reviewed ceiling/turn/return/landing sheets. These are wall-time live runs, not pixel-matched
  deterministic frames or a continuous-motion visual recording.
- All capture/benchmark/live renderer strings verify llvmpipe/Pixman; kernel deltas are empty.
  No physical AMD GPU use or hardware/platform qualification. Single-map/driver limits remain.

### Shadow-focus performance and provenance

Raw `build/campaign-performance/shadow-focus-{before,after}`: identical 1280×720 overview,
VSync off, LP_NUM_THREADS=8, 120 warmup +600 samples. No other campaign build/tests/graphics
ran during either benchmark or the sequential live comparison. Direct game time -v includes
startup/warmup; RSS is process memory, not GPU VRAM.

| Metric | Before | After | Change |
| --- | --- | --- | --- |
| Mean sample ms | 87.5913 | 90.5600 | +3.39% |
| p95 ms | 97.9784 | 98.3808 | +0.41% |
| Game CPU user+system s | 377.10 | 401.89 | +6.57% |
| Peak game RSS KiB | 811492 | 812032 | +0.07% |
| Tracked steady scene submissions | 65–66 | 65–66 | unchanged |

Both have 179 palms, 192 canopies, 58 terrain cells/6 terrain draws. The map still uses the
same target/footprint and cached redraw policy, but its refresh now submits actual forest
casters rather than an empty map. Coarse live flight telemetry (6 five-second flight windows
in each scripted run) is 6.81 →6.64 FPS; this is a smoke comparison, not a fixed-route benchmark.
One software pair plus live telemetry does not establish hardware cost or a stable timing delta.

Before binary `build/campaign-shadow-focus-before-binary`; candidate
`build/campaign-shadow-focus-after-binary`. Benchmark run.json files record launch-time hashes.
The baseline live build directory contains symlinks to the real CMakeCache and immutable before
binary, retaining the same compiled source asset paths. Exact live paths:
- `build/campaign-shadow-focus-baseline/software-flight/20261101T234254.198115Z`
- `build/software-flight/20261101T234348.008955Z`

Before/after commands and binary hashes: `build/campaign-shadow-focus-live.log`. Both live
`kernel-final.json` files and all capture/benchmark kernel deltas are empty.
Reviewed sheets: `build/campaign-final-comparisons/shadow-focus.png`, `shadow-focus-live.png`;
matching JSON indexes retain source paths. Full static capture pairs under build/software-captures:

| View | Before UTC directory | After UTC directory |
| --- | --- | --- |
| overview | `20261101T233257.711913Z-overview` | `20261101T233920.205021Z-overview` |
| bay | `20261101T233313.876795Z-bay` | `20261101T233928.754408Z-bay` |
| flight detail | `20261101T233321.677453Z-flight` | `20261101T233937.964792Z-flight` |
| jungle walking | `20261101T233329.935217Z-jungle` | `20261101T233946.203690Z-jungle` |
| no sun shadows | `20261101T233305.455754Z-overview-no-sun-shadows` | `20261101T233954.674202Z-overview-no-sun-shadows` |

No temporary source override or pending process. Final binary/source identity recorded AFTER
qualification in `build/campaign-shadow-focus-final-identity.json`; the standard capture harness
still does not record executable hashes at launch. Build/capture logs use the
`build/campaign-shadow-focus-*` prefix. Branch develop, all work uncommitted; no push/merge.

Shadow-focus stage game binary SHA256: `8d15632a27c23cb7ffb63c3946563325ab2f32ec0510b90536c989bdf84b099a`.




## Continuation — canopy foot closure (qualified, 2026-11-02)

Observed: ordinary canopy trunks/buttress feet ended in a horizontal plane on slopes. Actual
near/far trunk-base audit found gaps up to .395115 m. WorldReport root-gap fields cover origins,
not whole footprints, so they alone did not expose this. Three full-size root baselines visibly
float; final paired root images visibly close those gaps.

- Added one narrow lower ring beneath every existing canopy trunk, including mangrove main
  trunks. Every original positive-height trunk ring/face remains in place: shared CPU bark
  queries and monkey grip geometry stay exact. Lower ring at local y=-.016 extends downward
  to the drawn terrain; uphill portions remain buried. No terrain/collision/layout change.
- Ordinary buttresses use tagged downhill relief. Mangrove bank stilts retain their existing
  two-direction bank adaptation/blend. One shared generated helper drives visible and sun-shadow
  vertices, with matching basal pigment reference and conservative .8/1.6 local-m root bounds.
  This is the custom instanced path. Stock fallback gets the nominal extra ring, without
  terrain conformance; no fallback visual qualification is claimed.
- Reused the existing --canopy-lod-inspection mechanism with an explicit roots tier. It forces
  the selected near mesh, uses a stable origin-based 4 m camera and freezes simulation/time.
  Exact before/after camera metadata matches; source JSON stores target IDs and cameras.
  No interactive camera/traversal changes. Inspection remains capture-only and diagnostic guards
  reject unrequested/mismatched modes. Main/renderer/harness accept near|far|roots.
- Added/extended regression coverage in RenderedTerrainSurfaceTests: all original bark vertices
  unchanged, closure ordering/bounds, 12240 foot samples against packed terrain, blend endpoints
  and locale-stable shader constants. Old floating observations >2 cm:4594 (not unique trees).
  Every sampled lower closure point is below the surface; highest gap -0.00336266 m. This is
  a CPU/packed-height reference audit, not direct GPU readback or a whole-surface proof.
- Static geometry: +12 near/+5 far vertices and +24/+10 triangles per canopy mesh. Native vertex
  payload3543852 →3551808 bytes (+7956B); existing52B layout/9attributes, VBO/resource counts and
  draw grouping retained. Ordinary distant range200 →210 triangles, mangrove340 →350. Existing
  indices grow by2376B including distant ranges. No global density/resolution/range increase.
- First material-tag candidate failed the explicit -3 allowlist and was corrected by admitting
  exactly that tag. The later foot-ring capture failed the old exact mesh fixture; after full
  image review it now checks5010/4888 near,1076/1130 far,distant210. Finite/UV/membership and GPU
  safety checks stay intact. Failed gates at 20261102T002912.798463Z-canopy and
  20261102T005847.910326Z-canopy are retained, kernel-empty application/fixture failures.
- An earlier direct-trunk deformation looked good but was superseded because it could change
  the shared bark contact mesh. Its timing/photos are intermediate only (sheets have
  -intermediate copies). The delivered closure keeps all original trunk contact surfaces and
  adds geometry below the original foot; do not present intermediate images as final.

### Final qualification

Release/OpenGL33 builds pass with taskset0–3/-j3. C++ **77/77**,112.71s,
`build/campaign-root-closure-final-cpu.log` (overlapped captures; not a speed comparison);
Python **59/59**, matching-python.log. World report
`build/campaign-root-closure-world-final.json` remains parsed-identical, fingerprint238be1fec0a0cdd7.
Same36 display-tagged tests and2 orphan validators excluded; supervised paths used instead.
All9 final capture gates pass on runtime-confirmed llvmpipe/Pixman; kernel deltas empty.

Reviewed all three root comparisons, environment sheet, representative full images and the
monkey-climb image/gate. Root gaps close; saplings, mangroves and nearby ground plants remain
coherent. New tree-root geometry/shadows naturally alter some background pixels in the ground
views, so these are not claimed byte-identical controls. Layout, collision and procedural report
stay identical. No physical AMD GPU, alternate-backend, Web/Android or fallback validation.

Final reviewed sheets: `build/campaign-final-comparisons/trunk-roots.png` and
`trunk-root-environment.png`, with JSON source indexes. Exact root pairs under build/software-captures:

| Target X:Z mm | Before UTC directory | Final after UTC directory |
| --- | --- | --- |
| -114585:71883 | `20261102T002137.831308Z-canopy` | `20261102T010115.962839Z-canopy` |
| -39538:-12123 | `20261102T002147.687793Z-canopy` | `20261102T010129.174107Z-canopy` |
| -116543:50525 | `20261102T002157.308063Z-canopy` | `20261102T010142.222324Z-canopy` |

Normal before views use immutable `build/campaign-root-before-binary` (QA extension only) via
`build/campaign-root-before-build`, with same source assets/CMake cache. Before directories:
- `build/campaign-root-before-build/software-captures/20261102T002848.428891Z-jungle`
- `build/campaign-root-before-build/software-captures/20261102T002856.414451Z-mangroves`
- `build/campaign-root-before-build/software-captures/20261102T002904.157049Z-sapling`
Ground/litter baseline under build/software-captures: `20261102T001331.600426Z-ground`,
`20261102T001349.410513Z-litter`. Final after directories (same parent):
- `20261102T010154.717798Z-jungle`, `20261102T010207.427856Z-mangroves`
- `20261102T010220.142722Z-sapling`, `20261102T010233.215897Z-ground`
- `20261102T010246.531025Z-litter`, `20261102T010258.967558Z-monkey-climb`

### Final closure performance

Comparable serial jungle pair, same1280×720/LP8/VSync off/120warmup+600samples, direct game
/usr/bin/time -v, approved private llvmpipe supervisor. No concurrent campaign builds/tests/other
graphics. `build/campaign-performance/root-closure-{before,after}`:

| Metric | Before | Final closure | Change |
| --- | --- | --- | --- |
| Mean sample ms |174.7976|173.9644|-0.48%|
| p95 ms |191.5170|191.1788|-0.18%|
| Game CPU user+system s |609.57|597.33|-2.01%|
| Peak process RSS KiB |823876|814336|-1.16%|
| Tracked scene submissions |127–133|127–133|same range|

Both report105palms,32canopies,23terrain cells/10terrain draws,1487grass clusters. All kernel
deltas empty. Differences are within observed software timing variability, not a hardware or
stable speedup claim. RSS is process memory, not GPU VRAM. Earlier direct-warp candidate pairs
(root-ground-*) ranged157–217ms; the apparent initial28% speedup was not attributed to the change
and prompted reverse-order investigation. Those are NOT final-closure qualification numbers.

At the root-closure checkpoint, measured binary `build/campaign-root-closure-binary` matched
build/island-simulator. Benchmark run.json records launch identities. At that stage the standard
capture harness still lacked launch-time
executable hashes; post-run final binary/source checkpoint is
`build/campaign-root-closure-final-identity.json`. No temporary source overrides, pending process,
commits/push/merge or external dependency edits. Existing GPUVM incident remains open.

Root-closure stage binary SHA256: `4a9e6d0a66d793b7f70a2459df9b41c52254675dcc05c86686b999c94df440e6`.


## Continuation — bear facial fur and capture provenance (qualified, 2026-11-02)

- Actual before images showed long directional hairs wrapping across the face loft into
  horizontal forehead bands. A cropped short-fur sample made broad wrinkles and was rejected
  (`campaign-bear-material-after.log`,015610/015631 images). Final FaceFur material keeps the
  original coat palette/full source window but exchanges the loft's texture axes so hairs follow
  its muzzle direction. Only the existing face bridge uses it; whole-body UVs, geometry, rig,
  motion, silhouettes, nose and other facial material windows are retained.
- Full lying/walking/rain images and four-pair sheet inspected: forehead bands are reduced and
  coat grain sits more coherently with the skull. Pixel changes are confined to the face
  (walking bbox737,273..895,429; resting273,242..458,424 at1280×720). Outside these bounds RGB
  pixels match exactly in each pair. The coarse overall128px coat remains stylized; no claim
  that all fur appearance defects or platform differences are eliminated.
- Same6,382 near body triangles,2draws in all paired inventories,128×128 texture/8mips/87,380RGBA8B.
  No added vertices/buffers/textures/draws or new sampler/shader. No new frame-time speedup or
  hardware performance claim; this is one small local material-coordinate correction.
- Static capture now persists executable artifact identity BEFORE starting the workload, plus
  harness/CMakeCache identities, and records executable identity again during cleanup. Both
  static and existing fullscreen workflows compare SHA256/size, expose artifact_content_unchanged,
  and reject changed/missing binaries. Secondary artifact errors are retained without erasing a
  prior workload error. Existing fullscreen already recorded a before identity; now it checks
  the after identity too. Shared verifier reuses existing artifact_identity; no new dependency.
- Checks are file-content before/after consistency, NOT executable pinning or kernel-level
  exec attestation; transient replacement followed by restoration between checks is outside
  their guarantee. Replacing a file with identical bytes is allowed; both file identities stay
  in the report. Earlier campaign images remain historical post-run attribution where noted;
  no hashes were retroactively added to their run.json files.
- Added CPU tests for stable content, same-size mutation, resized content, identical-byte atomic
  replacement and missing files. Isolated mocked static/fullscreen workflow tests exercise
  persistence before launch, failure cleanup, preservation of primary errors and rejection even
  after fake worker success. All mutation fixtures use temporary files; no real executable or
  GPU workload is modified/injected by these tests. Initial fullscreen fixture used the real
  harness root for locking; corrected to a private temporary root before accepting the tests.

### Validation and evidence

Release/OpenGL33 build passes with taskset0–3/-j3. Final non-display CTest **77/77**,116.81s,
`build/campaign-bear-flow-final-cpu.log`; Python **64/64**,
`build/campaign-provenance-final-python.log`. Same36 display entries/2orphan validators excluded.
World report `build/campaign-bear-flow-world-final.json` is parsed-identical,
fingerprint238be1fec0a0cdd7. CPU duration overlapped captures and is not a speed comparison.

All8 final static runs pass with verified llvmpipe/Pixman, matching artifact before/after hashes
and empty kernel deltas. Walking bear animation-stage gate passes. Final live fullscreen/input
run also passes the existing input/window/performance-window gates and new artifact guard:
`build/software-fullscreen/20261102T021146.478436Z`; kernel-final.json empty. No physical AMD GPU,
other backend/platform qualification, driver work or external dependency source changes.

Final reviewed sheet `build/campaign-final-comparisons/bear-face-flow.png`; its JSON records exact
paired paths, executable hashes and changed bounds. Before uses immutable root-closure binary
via build/campaign-bear-flow-baseline (same CMake cache/source assets); hashes are now recorded by
the harness in each actual run.json. After uses current build/island-simulator.

| View | Before directory | Final after directory |
| --- | --- | --- |
| bear | `/rv/data/development/github.com/libcnalab/island-simulator/build/campaign-bear-flow-baseline/software-captures/20261102T020943.571017Z-bear` | `/rv/data/development/github.com/libcnalab/island-simulator/build/software-captures/20261102T021046.350735Z-bear` |
| bear-lying | `/rv/data/development/github.com/libcnalab/island-simulator/build/campaign-bear-flow-baseline/software-captures/20261102T021006.543735Z-bear-lying` | `/rv/data/development/github.com/libcnalab/island-simulator/build/software-captures/20261102T021112.322906Z-bear-lying` |
| bear-lying-showers-wet | `/rv/data/development/github.com/libcnalab/island-simulator/build/campaign-bear-flow-baseline/software-captures/20261102T021018.009981Z-bear-lying-showers-wet` | `/rv/data/development/github.com/libcnalab/island-simulator/build/software-captures/20261102T021123.860685Z-bear-lying-showers-wet` |
| bear-lying-t06.20 | `/rv/data/development/github.com/libcnalab/island-simulator/build/campaign-bear-flow-baseline/software-captures/20261102T021030.423945Z-bear-lying-t06.20` | `/rv/data/development/github.com/libcnalab/island-simulator/build/software-captures/20261102T021135.404062Z-bear-lying-t06.20` |

Exact build/run logs: build/campaign-bear-flow-final-build.log,
campaign-bear-flow-final-captures.log. No temporary material candidate, source override or pending
process. Final binary/source checkpoint: build/campaign-bear-provenance-final-identity.json.
Branch develop; all work uncommitted, no staging/commit/push/merge.

Current final game binary SHA256: `979523f0d8023347d6825a20de67c413b99d1a75a6a48938d1b4bb344eaea129`.


## Continuation — mine approach material and slope lighting (qualified, 2026-11-02)

- The actual entrance and side views exposed rectangular colour seams in the cutting's gravel
  and an abrupt join to forest substrate. `MineRenderer.cpp` now uses its existing smoothTri
  path with corner pigment and normals sampled from the same cutFloor support. It reuses
  TerrainSurfaceFrame normalization; original corner positions, tessellation, winding, culling,
  support/collision, lanterns, rails and interior remain intact. Shared corners shade identically.
- TerrainRenderer reuses the existing mine frame and footprint-filtered hummock/medium fields
  for a short tapered compacted-earth approach, preserving most substrate texture. No new noise
  call, texture, uniform, geometry, buffer or draw. The treatment ends outside the portal and
  has a height bound, preserving natural cover over the buried mine. Existing sun/weather/wetness
  treatment follows it. First broad/soft earth patch was narrowed and reduced after image review;
  its025221/025230/025240 intermediate captures are not final qualification.
- Base mesh stays22,304triangles. Slope-correct normals cause two more existing floor triangles
  to exceed the existing lamp-contribution threshold: additive22,479→22,481triangles, same passes
  and buffers. No mesh/population increase to conceal the material problem.

### Validation and images

Release/OpenGL33 final build: `build/campaign-mine-final-build.log`, same taskset0–3/-j3 limit.
Final non-display CTest77/77,87.46s (`build/campaign-mine-final-cpu.log`); Python64/64
(`build/campaign-mine-python.log`). Same36 display entries/two orphan validators excluded.
World report parsed-identical to the preceding bear stage, fingerprint238be1fec0a0cdd7:
`build/campaign-mine-world.json`. Test duration overlapped captures and is not a performance metric.
No new input/traversal behavior or live-control qualification is claimed by these local material tests.

Fourteen accepted captures form seven exact-view pairs: mine front/side, rain/wet, dawn06.2,
forest control, overview and adit interior. Reviewed whole images and contact/detail sheets:
`build/campaign-final-comparisons/mine-grounding.png`, `mine-detail.png`.
`mine-grounding.json` records every exact source directory, before/after executable SHA256,
mesh inventory and changed bounds. Jungle and adit RGB pixels match exactly; overview differences
are confined to631,347..639,351 at the entrance (1280×720). Front/cutting images show removal
of quad-colour discontinuities and a restrained earth transition; the gravel remains stylized.
All14 run.json files have matching before/after artifact hashes and no failure; actual runtime
llvmpipe and compositor Pixman verified, kernel deltas empty. Logs: campaign-mine-before-qualified.log,
campaign-mine-final-captures.log. Initial reference-copy placement lacked sibling Content assets;
its five failed runs in campaign-mine-before.log were corrected by moving the saved binary beside
Content and linking it from the baseline directory. Those failed runs are not qualification.

### Comparable software performance

Serial same1280×720 mine viewpoint,120warmup/600sample intervals, VSync off, existing supervised
software benchmark helper; no build/test/capture overlapped either benchmark. Actual llvmpipe/Pixman
and empty kernel deltas verified. Records: `build/campaign-performance/mine-before/` and
`mine-after/`; extracted metrics in `build/campaign-mine-performance.json`.

| Metric | Before | After |
| --- | --- | --- |
| Mean sample ms |169.8705|167.4462|
| p95 ms |222.2385|186.3249|
| Game CPU user+system s |679.77|649.30|
| Peak process RSS KiB |875304|874988|
| Tracked submissions |129–131|129–131|

Both retain79palms,91canopies,1150grass clusters and24terrain cells/8terrain draws; the scene-range
reports are identical. Mean−1.43%,CPU−4.48%,RSS−0.04%; timing variability is visible (especially
baseline p95), so these are observations, not a proven speedup or hardware cost qualification.
RSS is software-process memory, not GPU VRAM. First-draw shader compilation is not warmed cost.
Benchmark launch identities remain in their run.json; content was checked again after both runs.

Final binary SHA256: `221721fdc8f1a75ea963fe1630ad479780a5eafe9e0b98d64a06c49c480055b0`.
Baseline saved as `build/campaign-mine-before-binary` (prior979523f0…binary, same Content assets).
Final checkpoint: `build/campaign-mine-final-identity.json`; no pending process or temporary source
candidate. develop;33tracked modifications plus4new files, all uncommitted; no staging/push/merge.
No external dependency source or system configuration changes. Physical AMD GPU remains untested,
GPUVM incident OPEN; other backends/platforms remain unqualified.


## Continuation — land and reef rock basal grounding (qualified, 2026-11-02)

- Actual low-angle `rock-contact` baseline revealed a hollow underside above the slope. Initial
  default-world rim audit found102/204rock placements with gaps>2cm, worst0.681m. The shared
  RockSurface now lowers only its original basal ring to−1.25local metres; all upper-ring/crown
  positions, placement anchors, perches, obstacle layout and topology remain. Normals are rebuilt
  by the existing area-weighted method. Near/far retain265/36vertices and504/63triangles per variant;
  entire prop inventory stays12,780vertices/357,840vertex bytes, same buffers/layout/draw groups.
- NaturalPropRenderer bounds include the scaled buried rim in visible and existing shadow paths.
  Reef habitat variant3 reuses the same surface: its lower/upper/radial culling extents now use
  the shared rock bounds. Other coral forms remain unchanged. No new shader, texture, sampler,
  instance attribute, terrain pipeline, external dependency change or population increase.
- Added one fixed `rock-contact` viewpoint to the existing CLI/software capture path. It selects
  the largest ORIGINAL rim-to-heightfield gap and looks uphill from below; selection ignores the
  corrected mesh Y, so both image cameras remain identical. CLI help/name switches are complete.
  Initial compile caught the missing Main name-switch case; corrected before accepting baseline.

### Regression and visual qualification

Final Release/OpenGL33 build `build/campaign-rock-bounds-final-build.log` passes with taskset0–3/-j3.
Final non-display CTest77/77,93.76s: `campaign-rock-qualified-cpu.log`; Python64/64:
`campaign-rock-qualified-python.log`. Same36 display entries/two orphan validators excluded.
World report `campaign-rock-qualified-world.json` parsed-identical to mine stage,
fingerprint238be1fec0a0cdd7. Durations overlap captures and are not performance comparisons.

Registered rendered-terrain test now checks703,080basal edge samples across both LODs, land/reef
placements and five accepted prop seeds. Old mesh override fails (expected exit1) with highest
edge+0.730391m; corrected mesh passes, highest edge−0.0744905m. Logs:
`campaign-rock-qualified-red.log`, `campaign-rock-bounds-green.log`. CoreTests changes only its
existing bounds assertion; its count/topology/independent-normal checks still pass. Supplemental
CPU-only `campaign-rock-local-patch-audit.log` repeats all703,080rock samples using the actual
StreamBedPatch/WreckScourPatch replacements as well as the base mesh; same passing maximum.
Temporary reference/audit sources and binaries are in ignored build/, not application overrides.
At that checkpoint, exploratory seeds2,17,93219 were rejected by the preexisting turtle-trace placement condition;
these are not accepted seeds or a claim of arbitrary-seed generator qualification. Tested seeds:
DefaultSeed,0,1,0xffffffff,0x75a81423. Sampled edge coverage is not a proof for every possible seed,
all distant fade scales, platform or future authored terrain change.

Twenty accepted images form ten reviewed pairs: dry/wet rock-contact, headland, east/west,
props, reef, jungle, overview and reef-habitat. Whole images and sheets inspected:
`build/campaign-final-comparisons/rock-grounding.png`, `rock-contact.png`. The matching JSON index
records exact source directories, hashes, changed bounds and mesh inventories. Final captures:
`campaign-rock-qualified-captures.log`; earlier three baseline reef/jungle/overview paths are in
`campaign-rock-final-captures.log`, other baselines in campaign-rock-inspection/contact-before.log.
Other AFTER images from contact-after/final-captures are intermediate, preceding final reef bounds.
West is pixel-identical. Jungle changes436pixels at small stones/contact; an earlier commentary
claim of an identical jungle control was corrected after exact comparison. Headland changes918,
overview385pixels; broad bounding boxes reflect scattered rocks, not whole-scene alterations.
All20 final paired run.json files have unchanged before/after executable content, four frames/four
updates, verified llvmpipe/Pixman and empty kernel deltas. No new live-input/traversal run is claimed.

### Comparable software performance

Serial1280×720 headland,120warmup/600sample intervals, VSync off, existing isolated software
supervisor, no overlapping campaign build/test/capture. Both actual llvmpipe/Pixman and empty
kernel deltas verified. `build/campaign-performance/rock-headland-before/` and
`rock-headland-after/`; extracted `build/campaign-rock-performance.json`.

| Metric | Before | After |
| --- | --- | --- |
| Mean sample ms |99.8707|100.3374|
| p95 ms |110.2339|108.3340|
| Game CPU user+system s |472.48|469.84|
| Peak process RSS KiB |823532|826124|
| Tracked submissions |70–72|70–72|

Mean+0.47%,CPU−0.56%,RSS+0.31%. Both retain59rocks,131palms,41canopies,183reef habitats,
55terrain cells/8terrain draws. Wildlife timing varies slightly; no speedup/hardware claim.
RSS is software-process memory, not GPU VRAM. Benchmark launch identities were checked again
unchanged after completion. Final checkpoint: `build/campaign-rock-qualified-identity.json`.

Current binary SHA256: `579c13061c744f74d9f1d147e9c011973b774f4f0221786b9e5ad1fe3acfc05d`.
Saved baseline `build/campaign-rock-before-binary`: b7990cc6… (original rock geometry plus QA camera).
Four initial environment baselines use prior221721fd…binary, same Content/world/render settings.
Branch develop;37tracked modifications plus4new files, all uncommitted; no staging/push/merge,
external dependency/system configuration edits or pending process. Physical AMD GPU untested,
GPUVM incident OPEN; other backends/platforms/stock fallback remain visually unqualified.


## Continuation — rain shelter and close drip cohesion (qualified with image-profile gap, 2026-11-02)

- Actual hut-eaves baseline showed broad grey near-camera drip ribbons. EaveDripRenderer now
  measures distance to the falling drop rather than its roof source and shares the ordinary
  rain's existing narrow width/near-eye opacity through RainStreakAppearance.hpp. Canopy drips
  reuse the same rules. Eave/canopy fall timing, sources, gravity, streak lengths, splash rings,
  colours at full visibility, weather density and existing buffer capacities remain; no added
  draw, shader, texture or renderer dependency. The ordinary rain formula is unchanged.
- RainRenderer's old shelter test excluded a streak only when its START lay below the roof.
  GabledRainShelter clips the finite segment's uncovered prefix against the existing gabled
  volume (including ends/eaves). Both ordinary and lamp-lit rain use that length. Six cached
  half-space clips, no new weather/occlusion engine. Existing hut footprint/calibration retained.
- Added one fixed hut-eaves inspection camera to the existing CLI/software path. It inspects
  under the overhang beside the wall; it is not a new player route through the decorative hut.
  Before/after use the same four updates/frames and0.4s clock. Initial clip-only eaves captures
  were pixel-identical: these phases did not visibly expose the roof hit. No visual improvement
  is claimed from those clip-only images; shelter geometry has separate CPU regression evidence.

### Validation and reviewed images

Release/OpenGL33 builds campaign-rain-camera-build.log, campaign-rain-drip-build.log and
campaign-rain-final-build.log pass with taskset0–3/-j3.
Normal CMake regeneration relinked the existing CNA runtime; no dependency source was edited.
Final non-display CTest78/78,95.35s (`build/campaign-rain-final-cpu.log`); Python64/64
(`campaign-rain-final-python.log`). Same36 display entries/two orphan validators excluded.
New portable CPU test is registered outside the CNA-dependent test section.141,636streak segments
exercise retained-prefix safety, analytic ridge/slope/eave/end contacts and rigidly transformed
huts;16,988fixtures reproduce the old start-only exclusion leak. Also protects narrow projected
width/near-eye fade and finite inputs. An initial rotation-sweep failure landed exactly on a
closed end boundary, where floating-point translation can change inside/outside classification;
interior/exterior sweep points were offset, while exact local contact fixtures remain. Final
unit log: `campaign-rain-drip-unit.log`. World report campaign-rain-world.json is parsed-identical,
fingerprint238be1fec0a0cdd7. No new live-input or physical-GPU qualification claimed.

Eight accepted pairs/16captures: storm/showers/evening eaves, storm hut front/rear-material,
storm jungle/litter and dry eaves. Actual full frames and sheets reviewed:
`build/campaign-final-comparisons/rain-eaves.png`, `rain-cohesion.png`; rain-cohesion.json records
exact paths, binary hashes, changed counts/bounds. Visible broad near eave bars are replaced by
finer/fainter drops; hut materials and surroundings remain. Jungle/litter/dry-eaves controls are
pixel-identical at these phases. All16 run.json files have matching before/after executable content,
verified llvmpipe/Pixman and empty kernel deltas. Logs: campaign-rain-before.log,
campaign-rain-drip-after.log, campaign-rain-detail-captures.log, campaign-rain-extra-qualified.log.

Separate ground+thunderstorm+wet-ground baseline AND after safely rendered but FAIL the existing
close-natural-detail image gate: luminance deviation9.4215/9.4211 versus required10.0. Both images
manually inspected; small drop differences are recorded in campaign-rain-ground-diagnostic.json.
These two are NOT accepted qualification, thresholds/goldens were not weakened, and this remains
an image-profile qualification gap. Baseline directory045410.796041Z-ground-thunderstorm-wet
under campaign-rain-baseline/software-captures; after045727.138420Z-ground-thunderstorm-wet under
software-captures. Both have unchanged executable content and empty kernel deltas.

### Comparable software performance and tail investigation

Same1280×720 hut-eaves thunderstorm/wet-ground,120warmup/600samples, VSync off,LP workers8,
existing contained software supervisor. No build/test/capture overlapped these four benchmarks.
First p95 rose10.57%, triggering the reverse-order repeat (after then before); it did not persist.
Directories under build/campaign-performance/rain-hut-{before,after,after-repeat,before-repeat}.
Extracted metrics: build/campaign-rain-performance.json.

| Metric | Before | After | After repeat | Before repeat |
| --- | --- | --- | --- | --- |
| Mean ms |110.1211|112.8760|113.7417|115.9192|
| p95 ms |119.0596|131.6398|123.4784|135.3056|
| Game CPU s |456.97|469.40|473.78|480.24|
| Peak process RSS KiB |723724|720988|719388|720588|
| Tracked submissions |102–103|102–103|102–103|102–103|

First mean+2.50%; reverse-order comparison−1.88%. Timing varies, so no speedup or persistent
regression claim. Buffer/mesh budgets unchanged; RSS is software-process memory, not GPU VRAM.
All four verify actual llvmpipe/Pixman, empty kernel deltas, unchanged executable AND same CNA
runtime content before/after. Script is ignored build/campaign_rain_benchmark.py, reusing the
existing supervisor with explicit weather flags and added artifact/runtime checks.

Final binary SHA256: `0ef6c2a03efeba58d2f23a78f3d52f1dc106e472289277d748afe05da5f4c578`.
Immutable baseline: build/campaign-rain-before-binary (original effects plus eaves camera).
Final source/runtime checkpoint: build/campaign-rain-final-identity.json. No pending process,
temporary source override or disabled effect. develop;41tracked modifications plus7new files,
all uncommitted; no staging/commit/push/merge. GPUVM incident OPEN, AMD physical GPU untested;
other backends/platforms remain unqualified. Existing external issues in BUGS.md remain unchanged.


## Continuation — deterministic turtle nesting recovery (qualified, 2026-11-02)

### Implemented and bounded

NaturalPropLayout's original36radial candidates could fail its constructor after committing only
one provisional trail. Original CPU regression reproduces `Unable to place both turtle nesting
traces on dry sand` (campaign-turtle-nesting-red.log); contained seed2 startup also fails before
graphics initialization (campaign-turtle-startup-red.log). That startup has no image and is NOT
software-renderer qualification. The broader original-code audit finds17failures in67sampled
layouts: seeds0–63,93219,DefaultSeed andUINT32_MAX.

Successful original layouts retain the exact original36-ray search and placement arithmetic.
Only a failed pair is rolled back atomically to its pre-trace beach-detail count. Bounded recovery
uses281nest rays inside the same authored western beach interval and nine approach headings,
including modest oblique crawls; at most2529recovery candidates. Original dry sand, height/slope,
route, tree/detail and18m nest-separation gates remain. Whole crawl footprints, including their
rendered half-segment extensions, additionally avoid rocks, exposed roots, fallen palms and wood.
Wood uses its documented local-X heading and a capsule covering bends/forks, leaving adjacent
sand available. No coastline expansion, relaxed habitat gate, prop removal or new geometry engine.

A first fine-ray-only attempt constructed the three known failures but manual images exposed
rock/wood crossings. Its centre-only test fails the added footprint regression
(campaign-turtle-footprint-red.log); that implementation was superseded. Oversized circular wood
bounds and radial-offset trials failed safely and were not shipped. Final recovery uses oblique
approaches with footprint checks. Existing valid layouts were deliberately not retrofitted;
this does not claim every historic trace is free of all preexisting clutter overlaps.

### Visual evidence and explicit scope

`python3 tools/run_software_capture.py build turtle-nest --natural-props-seed 17` is a capture-only
local prop-layer preview. It renders alternate props, their contact shadows, turtle traces and
associated nest/surf effects against the canonical gameplay world. Collision, landmark siting and
other authoritative world systems keep their original layout. It is NOT full-world seed support
or alternate-world traversal qualification. The main executable rejects ordinary/benchmark use;
the supervisor records the seed/scope and rejects missing, mismatched or unexpected seed markers.
This preserves all ordinary feature/renderer/image gates. Two Python regressions protect scoping.
An initial full-world override failed the canonical jetty gate and was replaced by this explicit
preview, without weakening that gate. The final preview camera looks from the open coast along
the actual approach, showing the crawl and nest without hiding neighboring objects. Ordinary
fixed cameras are unchanged. Earlier low/overhead preview cameras were superseded for occlusion.

Seven accepted captures: two original day/dawn baselines plus five final views (matching ordinary
day/dawn controls and local seeds2,17,93219). Actual final full frames and contact sheets reviewed;
recovered crawls occupy open sand beside rocks/wood/roots, rather than crossing their bodies.
The ordinary day/dawn images are pixel-identical; their preexisting clutter is preserved.
Original failing seeds have no baseline image, so no matching before/after visual improvement
is claimed for those seeds. Reviewed evidence:
- build/campaign-final-comparisons/turtle-default-controls.png
- build/campaign-final-comparisons/turtle-seed-inspections.png
- build/campaign-final-comparisons/turtle-nesting.json (exact paths, hashes and preview scope)

All seven run.json files retain matching pre/post executable content, actual llvmpipe/Pixman
verification and empty kernel deltas. Final capture log: campaign-turtle-qualified-final-captures.log.
Rejected intermediate captures remain identified in their separate logs, not in the final index.

### Validation, performance and remaining boundaries

Release/OpenGL33/GCC14.2 build passes: campaign-turtle-final-build.log and
campaign-turtle-camera-final-build.log; final test-only build campaign-turtle-audit-test-build.log.
All compilations were pinned to CPU0–3 with taskset/-j3 (direct reference builds likewise).
Final safe CTest79/79,105.15s (campaign-turtle-qualified-final-cpu.log); Python66/66
(campaign-turtle-qualified-final-python.log). Same36display tests/two orphan validators excluded.
Commands: `ctest --test-dir build -LE display -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2`,
`python3 tests/SoftwareCaptureTests.py`, `build/island-world-report --output <report.json>`.
Parsed final world report is identical to rain checkpoint, fingerprint238be1fec0a0cdd7.

New portable IslandTurtleNestingTests covers21layouts: all17reproduced failing seeds and four
legacy/extreme controls. It checks complete pairs, habitat/spacing/support, repeated bitwise
placement and an independently oversampled whole rendered crawl, including end extensions,
against roots/rocks/wood. Final unit log: campaign-turtle-final-unit.log.
Independent original/current constructor audit:67/67now construct two nests/14marks; all50legacy
successes retain identical beach-detail field fingerprints. Logs campaign-turtle-legacy-audit.log,
campaign-turtle-final-audit.log and campaign-turtle-audit.json. This finite audit is not a proof
for arbitrary uint32seeds, future terrain edits or alternate platforms.

Comparable CPU-only constructor benchmark, serial CPU0,2warmup/12samples, same Release-O3:
default mean219.244→213.321ms (−2.70%), median218.691→213.103ms. This is timing variability,
not a speedup claim; no other campaign workload overlapped, but host scheduling is uncontrolled.
Recovered seeds2/17/93219 average292.168/283.786/303.037ms; formerly failing constructors produced
no comparable valid scene. The extra bounded search is loading work only. Terrain/palm/jungle
construction is excluded. No frame-time, GPU-cost or VRAM claim is made from these CPU timings.
Full metrics/executable identities: build/campaign-turtle-final-performance.json. Default world,
geometry and rendered controls are unchanged. No new frame benchmark was needed for unchanged
per-frame code in the ordinary world.

Final application SHA256:802f80a8696c5eb16264b7337719d6fb910efbf2f2300b99ad0090e6774382ef.
Final source/runtime checkpoint: build/campaign-turtle-final-identity.json. develop/8b84844e;
42tracked modifications plus8new files, all uncommitted. No staging/commit/push/merge, dependency
source edit, temporary production override, disabled effect or pending process. GPUVM incident
remains OPEN; no physical GPU or other renderer/platform qualification. Existing Android/CNA
issues and the prior ground-in-storm contrast qualification gap remain unchanged.


## Continuation — boar stride/cadence and live motion QA (qualified, 2026-11-02)

### Reproduced and corrected

BoarHerd advanced gait by7radians/metre although the posed stride shrinks at low speed and is
scaled for juveniles. A temporal audit of the real herd reproduced supporting hooves sliding
with the body:40,593steady stance pairs,28,337failures even at18%-of-body-travel plus20micrometre
tolerance, worst slip/body-travel ratio0.916667 (campaign-boar-gait-red.log).
BoarPose now shares the .245m half-stride, speed/amplitude mapping and distance-driven clock;
BoarHerd supplies accepted3D terrain travel and body scale. Rejected moves cannot advance gait.
GroundPose uses the same amplitude. Height is sampled once on accepted moves; blocked positions
retain their existing height. Rig/bone lengths, navigation/collision, anatomy, RGB coat, buffers,
mesh/material budgets and two renderer draws remain. Turns/acceleration/stops still use the
existing procedural pose; this is not world-space stance locking.

New portable IslandBoarGaitTests covers33,066flat stance intervals across six speeds/three scales
(<4micrometre displacement each) and40,135live steady pairs (25,328adult/14,807juvenile). No failure
at the TIGHTER5%-of-body-travel plus20micrometre tolerance; worst ratio0.0227233. Also covers
invalid/stopped/blocked inputs and wrapped phase. A final tiny-negative-phase case reproduced
rounding to exactlyTau; normalization now excludes that endpoint (campaign-boar-gait-wrap-red.log).
Final unit evidence: campaign-boar-gait-qualified-unit.log. Existing fixed-joint/contact audit
also passes10,368poses/28,512contacts. No exact locking on changing slopes/turns/stops is claimed.

### Actual rendering and matching baseline

The old eight-stage QA used an invalid observer, selecting Dormant simulation and freezing the
animal between distant updates. The capture-only branch now observes20m overhead: Full simulation
without a close threat. No teleport/forced behavior/hidden vegetation or gameplay observer change.
Stages report actual tier; the Python gate rejects Reduced/Dormant/missing tiers and motionless
sequences, retaining ordered clock/finite-pose gates. Corruption and overhead-policy tests pass.

Initial dormant-QA/first cadence-only images are intermediate (campaign-boar-flow-before.log,
campaign-boar-gait-after.log), not final motion comparisons. The matched reference links ORIGINAL
BoarHerd from8b84844e before the core archive, with the SAME final QA/rendering objects. Production
sources/executable/dependencies were never replaced. The binary remains beside Content, linked
from campaign-boar-gait-baseline. Exact source/object/binary identities and link arguments:
build/campaign-boar-gait-reference.json.

Four final pairs/eight runs: day, lying control, showers/wet, dawn. The three motion pairs include
48stage images. Full day frames, all day phase pairs and environment/lying sheets were inspected.
Stance/swing timing is corrected while body routes stay identical; no new visible clipping or
floating in the reviewed images. Natural grass still partly occludes some feet; this is not
exhaustive continuous-motion/all-species acceptance. All paired position/speed/rooting/time
samples match exactly, all tiers are0 and sampled path length is3.7745m. Lying pixels are identical.
All56final full/stage images also match the reviewed pre-wrap images pixel-for-pixel.
Reviewed sheets/index:
- build/campaign-final-comparisons/boar-gait-start.png
- build/campaign-final-comparisons/boar-gait-walk.png
- build/campaign-final-comparisons/boar-gait-context.png
- build/campaign-final-comparisons/boar-gait.json (paths, bounds, trajectory/normalization controls)

Final logs: campaign-boar-gait-final-before.log / final-after.log. All eight executable pre/post
content guards pass, actual llvmpipe/Pixman is verified and kernel deltas are empty. No shader,
texture, material, mesh, draw or GPU-resource addition.

### Qualification, cost and boundaries

Release/OpenGL33/GCC14.2 builds pass (campaign-boar-gait-final-build.log,
inspection-build.log, wrap-final-build.log); every compile uses CPU0–3/-j3 or an equivalent
four-core-pinned direct reference build. Final non-display CTest80/80,116.30s:
campaign-boar-gait-final-qualified-cpu.log; Python66/66 with expanded tier/motion cases:
campaign-boar-gait-final-qualified-python.log. Same36display entries/two orphan validators excluded.
Commands: `ctest --test-dir build -LE display -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2`,
`python3 tests/SoftwareCaptureTests.py`, `build/island-world-report --output <report.json>`.
Parsed final world report is identical to turtle checkpoint, fingerprint238be1fec0a0cdd7.

Isolated CPU herd benchmark:30,000Full-tier updates/sample at120Hz,2warmup/7samples, serial CPU0,
no other campaign build/test/render job; no supplied obstacles. Mean2081.10→2151.37ms (+3.38%),
median2069.72→2043.35ms (−1.27%). Host scheduling is uncontrolled; this is variability, not a
speedup/stable regression claim. Earlier pre-wrap pair2441.54→2428.87ms (−0.52%) is retained in
campaign-boar-gait-performance.json; final values/identities in campaign-boar-gait-final-performance.json.
Independent30s audit hashes EVERY non-gait BoarState field/tick for six animals: both fingerprints
7aa3bd966b810979 and matching final-state checksums. Ordinary boars benchmarks use the nearby
observer and can stop the inspected animal, so this isolates active simulation cost. No new
sustained software frame-time, VRAM, GPU-cost or physical-hardware claim.

Final application SHA256:47e1178115b905c1fecb63515c8502c7fa2e3865a6bf4263cbf7f2710dbad019.
Source/runtime checkpoint: build/campaign-boar-gait-final-identity.json. develop/8b84844e;
47tracked modifications plus9new files, all uncommitted. No staging/commit/push/merge, dependency
source edit, production override or pending process. GPUVM/flight incidents stay OPEN; physical
GPU/other platforms remain untested. Prior storm-ground contrast qualification gap is unchanged.


## Continuation — deer body/hoof support (qualified, 2026-11-02)

Actual standing/bedded baselines visibly floated. The old renderer and shadow placed every ankle
.060model metres above the centre ground, although the toe's downward radius is only.024.
Original-code CPU regression reproduces a.036m flat-ground gap (campaign-deer-contact-red.log).
The saved original executable is build/campaign-deer-contact-before-binary, beside Content;
campaign-deer-contact-baseline links it without replacing production sources or the current app.

New portable DeerHoofSupport shares the rendered support plane and foot fitting between
DeerRenderer and WalkerShadow. The authored body follows the local plane and moves down the
ankle/radius difference before small joint corrections. Split toes follow their own local normal;
vertices/edge midpoints of BOTH6/8-sided LODs determine clearance against the actual cached mesh
and its local patches. Both LODs include the same downward radial vertex. Knee/hock corrections
are distributed through the existing procedural pose; stamps lift along the posed body-up and
bedding remains supported. Frustum centres follow the leaning body with the SAME2.1m radius;
distance/LOD budgets are unchanged. Dorsal/belly pigment uses body-relative normals, while direct
lighting keeps world normals. Same topology/counts, buffers, materials/textures, two draws and
population; no new rig, renderer or dependency. No deer AI, collision or placement change.

A foot-only prototype passed simple contact but needed up to.405m correction in the broader audit;
it was superseded by whole-body plane support. The first plane version mixed world-up hoof lift
with body-up joint lift; the pastern-length regression caught this and the stamp is now consistent.
These intermediates (campaign-deer-contact-after.log / deer-plane-captures.log) are not final
qualification. Final portable test:834poses/3,336feet, including translated flat planes, scale,
folding, stamping, habitat slopes and18drinking-bank poses with StreamBedPatch. Independent denser
mesh reconstruction checks both LODs, contact/airborne clearance and shadow agreement. Measured
planted gap range−0.000000477..+0.000001907m (2mm test tolerance); largest residual ankle correction
.135869m, main segment length change16.26%, pastern change.015805m/model scale. Bounded procedural
deformation, not a fixed-length IK rig. Source unit evidence: campaign-deer-support-stamp-unit.log;
final CTest executes the same test. Culling-limb bounds and invalid inputs also pass.

Seven accepted matched pairs/14runs: standing, bedded, showers/wet, dawn, west, clearing-edge,
jungle. Actual whole images and sheets reviewed; visible hoof gaps close, the bedded figure rests
on the slope and shadows follow the fitted pose. No new conspicuous clipping in the inspected
frames. West retains its intentionally partial foreground animal; it is not a full anatomy view.
Dawn changes include long projected animal shadows. Clearing-edge/jungle controls are exactly
pixel-identical. Same fixed cameras, seed, clock, profile and weather. Reviewed evidence:
- build/campaign-final-comparisons/deer-support-detail.png
- build/campaign-final-comparisons/deer-support-weather.png
- build/campaign-final-comparisons/deer-support-context.png
- build/campaign-final-comparisons/deer-support.json (exact paths, hashes and changed bounds)

All14static executable pre/post guards pass with actual llvmpipe/Pixman and empty kernel deltas.
Logs: campaign-deer-support-qualified-before.log / qualified-after.log. Deer views are existing
frozen pose inspections, not live gait acceptance; bank geometry has CPU coverage, not a new live
bank-traversal capture. Finite sampled coverage is not a proof for every future terrain/pose.

Release/OpenGL33/GCC14.2 builds pass (campaign-deer-support-final-qualified-build.log and earlier
deer-support/stamp build logs), all compilation pinned to CPU0–3/-j3. Final non-display CTest81/81,
95.13s (campaign-deer-support-qualified-cpu.log); Python66/66 (qualified-python.log). Same36display
entries/two orphan validators excluded. Commands remain `ctest --test-dir build -LE display -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2`,
`python3 tests/SoftwareCaptureTests.py`, `build/island-world-report --output <report.json>`.
Parsed world report remains identical, fingerprint238be1fec0a0cdd7; git diff --check passes.

Matched warmed software benchmark:1280×720/deer,120warmup/600samples,VSync off,LP workers8, serial
with no overlapping campaign job. Mean103.4102→104.5835ms (+1.13%); p95125.8546→119.1577ms (−5.32%).
Same2/4deer in two draws, tracked submissions118–120,15terrain cells,84palms,25canopies and1,161grass
clusters. Game CPU432.07→453.53s (+4.97%), peak process RSS787196→792080KiB (+0.62%); host timing
varies, no speedup/native-GPU/VRAM claim. Both benchmark executable AND CNA-runtime before/after
content guards pass, actual llvmpipe/Pixman verified, kernel deltas empty. Metrics/paths/identities:
build/campaign-deer-support-performance.json; build/campaign-performance/deer-support-{before,after}.
The ignored campaign_deer_benchmark.py reuses the established supervisor, without weather override.

Final app SHA256:fd3d7c46bd05d39d6d08a3623350bf635bb4e2f2152ee0d9dd74ae52f929bce0.
Source/runtime checkpoint: build/campaign-deer-support-final-identity.json. develop/8b84844e;
51tracked modifications plus12new files, all uncommitted. No staging/commit/push/merge, dependency
source edit or pending process. GPUVM/flight incidents remain OPEN; physical GPU and alternate
platforms/backends untested. Existing storm-ground contrast qualification gap remains unchanged.


## Milestone — terrain rock projection, 2026-11-02 (campaign remains ACTIVE)

Found a mathematical material defect: `mix(x,z,abs(x)/(abs(x)+abs(z)))` is identically
zero when x and z have opposite signs. Bedding, grain, joints and drainage therefore lost
horizontal variation across two quadrants. Existing macro/mottle samples already blended
results correctly; the finer material mixed coordinates instead.

Implemented `TerrainRockMaterial.hpp`: evaluate the existing mineral palette/detail separately
in XY and ZY, blend evaluated results with the existing smooth radial weights, offset the second
joint chart, and filter each with its full vertical-plane pixel footprint. Ground-plane exposure
shelves, heightfield, geometry, placement, collision, material abstraction and dependencies are
preserved. The focused extraction reduces TerrainRenderer.cpp rather than growing its shader
monolith. Both existing desktop/ES shader assembly paths include the helper; only desktop was
render-qualified. No new CPU test copying GLSL arithmetic was added; actual compiled shader
and matched rendered scenes provide the regression evidence for this material-only change.

Release/OpenGL33 build passed on CPU0–3/-j3 (`campaign-geology-build.log`).81/81 non-display
CTests passed75.60s (`campaign-geology-tests.log`);66/66 SoftwareCaptureTests passed.
World fingerprint remains238be1fec0a0cdd7 (`campaign-geology-world.json`).36 display CTests and
the two orphan display-output validators remain deliberately excluded; approved captures below
replace unsafe display launches. No hardware/platform qualification claim.

Six matched before/after pairs (east, headland, west, overview, tide-pools, rock-contact) all passed
unchanged capture gates, actual llvmpipe/Pixman, executable content guards and kernel monitoring.
Full images and detail/contact sheets were visually reviewed. The effect is deliberately modest:
less continuous cliff scoring and irregular bedding/outcrop detail, no new projection seams,
unchanged rock seating/shore pools. Distant overview changes are very small; this is not a claim
that all island geology has been transformed. Artifacts:
`build/campaign-final-comparisons/geology-projection.png`, `geology-projection-detail.png`,
`geology-projection-near.png`; exact first-four pair paths/changed counts in geology-projection.json.
Near pair paths in `build/campaign-geology-near.log`; baseline binaries retained as
`build/campaign-geology-before-binary` and alias directory campaign-geology-baseline.

Comparable private software benchmark: headland1280x720,120warmup/600samples, no VSync,
8 llvmpipe workers. Mean82.6978→83.2917ms(+0.72%),p9592.2332→92.8125ms(+0.63%).
Same70–72 tracked submissions,8 terrain draws/55cells,131palms,41–42canopy; no newgeometry.
User CPU371.41→381.01s(+2.58%), peak process RSS831784→827212KiB(-0.55%). Software-only,
not physical-GPU speed/VRAM. Full identities/metrics: build/campaign-geology-performance.json,
build/campaign-performance/geology-{before,after}. Current app SHA256:
4d2045b9c189f5ab1117bcde8a96a465b0968e682df82e7cf17e9e5d0000558e.

Next priority already started: latest forest audit of jungle/clearing-edge/stream/mangroves/
litter/bay/distant, all seven safe captures passed and contact sheet visually reviewed:
build/campaign-final-comparisons/forest-review.png/.json. Existing correlated stands, varied
crown heights, patchy ground layers, riparian colonies and open ridge/clearing transitions remain
functional; no evidence justifies replacing them or increasing total tree counts. One attempted
`canopy` capture stopped at argument validation because its explicit inspection target was omitted;
no application/GPU launch occurred. Add a proper target for the close tree audit. Still incomplete:
live deer gait (time-driven phase and no swing-foot clearance found in current source), broader
water/lighting/landmark/traversal/holistic criteria in the active ledger. Do not stop here.


### In progress — live deer gait (2026-11-02; NOT yet a qualified milestone)

Added deer-walk capture-only viewpoint (existing standing/bedded views retained):64fixed
updates,8images, real herd, following broadside camera, full-tier protocol validation. Initial
observer-overhead prototype provoked retreat because deer sense XZ proximity. Final diagnostic
observer is60m aside/20m above (inside90m full-simulation range, outside alert range); ordinary
AI/observer behavior unchanged. The discarded retreat prototype is not walking acceptance.

Implemented DeerGait.hpp distance/scale/surface-driven phase, four-beat walk → diagonal retreat,
stance/swing horizontal path and actual .095–.130m swing clearance through the existing joints.
Model and shadow share the same step evaluator; existing slope/hoof support reused. Stopped or
blocked travel does not tick the gait. No population/rig/collision/geometry-count change. No claim
of persistent world-foot locking during turns/acceleration.

Current focused CPU tests both pass. New actual shared-joint test:54890flat stance contacts,
31510swing samples, max flat world slip9.83025e-7m; live rendered-ground route4.65248m,
760moving frames/1630steady contacts/2840stopped frames, worst slip/body travel.000605085.
Full82-test non-display suite is running in campaign-deer-gait-tests.log;67Python tests pending
final rerun. Full source build limitedCPU0–3/-j3. Do not call this final until complete.

Matched old/new 8-frame walking/stop pair passed both safe renderer/image protocols. Sources
were byte-backed-up; an ignored script reverses ONLY owned gait hunks to build a comparison
artifact with the final identical QA observer, restores all current source bytes in finally, then
rebuilds the current app. Restoration hashes in campaign-deer-gait-source-backup/restored.json.
Artifacts: campaign-deer-gait-walk-before-binary and alias campaign-deer-gait-baseline; script
campaign_deer_gait_baseline.py. It is a temporary evidence tool, not production/test code.
Body position/speed/pose/tier/time match exactly across eight samples. Full first images and both
4-row detail sheets visually reviewed. The moving hoof now lifts/returns, stance reaches back;
stopped target is unchanged except float-rounding (24pixels at4s), other animals may still move.
Sheets/GIF/index: build/campaign-final-comparisons/deer-gait-{walking,settling}.png,
deer-gait-sequence.gif/.json. GIF is a finite6.4s recording with a stop, not a seamless gait loop.

Pending: additional weather/static control images, remaining support extremes, comparable
software performance, complete tests/world/source audit, then update this milestone and continue
water/lighting/landmark/traversal/holistic criteria. Forest close roots target-114585:71883 also
passed and its full image was reviewed: no remaining exposed basal gap. Current sources contain
the final gait, not a temporary baseline reversal. No build/render process should mutate that.


### Qualified — live deer gait continuation (2026-11-02; campaign still ACTIVE)

All pending milestone checks above now complete. Release/OpenGL33 build passed,82/82 safe
non-display tests passed121.11s,67/67 harness tests passed; fingerprint238be1fec0a0cdd7 unchanged.
Additional test-only support extremes then built/passed the two focused tests:867poses3468feet,
27bank poses, min-gap-1.90735e-6/max-gap4.76837e-7m, maximum residual ankle correction.139344m,
main-segment support change20.63% (under existing25% bound), pastern change.0161031m/model scale.
Tests retain the procedural rig; this is not fixed-length IK. Logs campaign-deer-gait-extremes-*.

Four matched capture runs/pairs: dry live walk/stop, showers live walk/stop, standing, bedded.
Each motion run contains8captured stages (32stage images across the four motion runs), all
reviewed through full initial frames and dry/rain sheets. Stance support and swing clearance
visibly improve; full target body/AI trajectories match in dry and rain. Static standing25changed
pixels/26total byte difference; bedded3pixels/8bytes (rounding). Controls sheet/index:
build/campaign-final-comparisons/deer-gait-controls.png/.json. Exact capture paths in
campaign-deer-gait-walk-pair.log and campaign-deer-gait-controls.log. All8runs pass unchanged
image gates, actual llvmpipe/Pixman, executable before/after content checks and kernel deltas.
Rapid retreat is CPU/support-tested, not a separate visual retreat qualification; turns and
acceleration may still slide briefly, as explicitly bounded in the helper comments.

Serial warmed private software benchmark, same deer-walk view1280x720/120warmup/600samples:
mean161.7113→158.2051ms(-2.17%),p95176.7515→177.3941ms(+0.36%). Same108–111tracked
submissions,14terrain cells/7draws,56palms,21canopy,1106grass clusters,1–4butterflies. Both
minimum frame times exceed the simulation's .1s clamp (before131.59ms), retaining comparable
live route behavior; this benchmark uses normal close-observer behavior, whereas captures use
the explicit distant QA observer. No geometry/draw-budget increase. User CPU704.24→687.38s,
peak process RSS800120→795196KiB. Not a hardware speed or VRAM result. Full data/identities:
build/campaign-deer-gait-performance.json and campaign-performance/deer-gait-{before,after}.
Current app14d10a6901da0a817d25b8cfaa7a96c4d251631cdaf26b8b8c7ad15c190193fe.

Moving to water/depth/coast/current surface-crossing review now. Subsequent lighting, wildlife
representative review, landmark and final traversal/holistic ledger rows remain incomplete.
No goal-complete call, commits, staging, pushes, dependency source or hardware changes.


### Continuing holistic review / current wet-material correction (ACTIVE)

Latest app14d10a...: seven water/coast/underwater views passed and contact sheet inspected:
build/campaign-final-comparisons/water-review.png/.json, exact runs campaign-water-review.log.
Water depth/clarity and rock/coral contact remain readable, no new plane seams observed.
Live private software --diving passed (software-fullscreen/20261102T112624.665620Z), both
surface/underwater images inspected; current tide-relative surface/dive/resurface trace passes,
max dive about1.873m. This is no full12-minute tidal-cycle or physical-GPU qualification.

Seven lighting views (overcast/dawn jungle,22:00 beach trail, storm/wet hut eaves, cave interior,
bunker corridor, overcast resident) passed unchanged image protocols and were visually reviewed:
lighting-review.png/.json and campaign-lighting-review.log. No obvious new seams, shelter-rain
leaks or material/exposure discontinuities observed in those scenes. Eight representative wildlife
views passed, including existing live bear/goat/boar8-stage protocols: crocodiles,turtles,birds,
herons,monkeys,bear,goats,boars. Wildlife sheet and24motion-stage contact sheet reviewed:
wildlife-review.png/.json and wildlife-motion-review.png. Fish/reef included above. Source/tests
retain bounded fish steering/pitch/bank/depth, bird routes and aquatic habitat/height rules;
static aquatic/bird shots alone are not claimed as continuous motion qualification.

Current focused issue: uniform30% wet-land absorption additionally darkens already dark organic
forest substrate, while wet grass/leaves only darken16%/12%. Storm ground image gate still fails
on the current baseline (software-captures/20261102T113605.331201Z-ground-thunderstorm-wet),
retained and manually inspected; existing threshold10.0 is unchanged. Preparing a material-aware
response using existing forest/turf/rock/cave/moisture fields, bounded16–30%, no new texture,
noise, geometry or lighting system. This is NOT yet built or visually accepted. Baseline app saved
campaign-wet-substrate-before-binary, control runs in campaign-wet-substrate-before.log; after
comparisons, software performance and regression checks pending. Do not stop at this checkpoint.

Wet-material trial1 rejected: broad humidity/substrate-dependent brightening did not resolve the
qualification gap and reduced whole-frame contrast (storm ground mean24.6462→26.5131,
std9.4213→9.1742, still below10). Full frame and five controls compared in
wet-substrate-trial.png. No threshold/golden changed. The trial's owned shader hunk was removed.
A second bounded material trial now distinguishes the existing source's pale leaf/wood fragments
from dark humus using the already filtered litter sample (no extra fetch/noise), retaining30%
on rock/cave/sand/fallback. Not yet visually accepted; do not claim a storm fix. Source image
and its code-luminance distribution were inspected (median.258,p90.454); source art unmodified.

Wet-litter trial2 now visually reviewed and passes the unchanged storm-ground image gate:
software-captures/20261102T114420.449437Z-ground-thunderstorm-wet, std10.34 versus10.0.
Uses existing filtered litter code-luminance to distinguish pale leaf/wood fragments from dark
humus; bounded absorption10–38% fades with imageFloor and excludes rock/cave surfaces. No
additional texture/noise/pigment/lighting term; dry response mathematically unchanged. Full frame
and same-size crop compared in wet-litter-ground.png: retained dark storm atmosphere, finer
organic fragments are distinguishable. Baseline9.4213 still retained as a genuine failed run,
not retroactively passed. Controls/performance/full final tests remain pending, so acceptance
is provisional. Source presently contains this second trial; the first broad brightening is gone.

Refined wet-litter composition: apply the absorption ratio to the litter contribution at its
original blend, then retain the original common30% wet-land multiplier. Later rock/soil/cave
material overrides therefore naturally keep their response, without new landmark masks.

Actual new weather defect found while inspecting the storm-cave baseline: material-side rain
impact rings appeared in covered cave-floor puddles, although the weather particle pass already
skips underground. Added rainExposure from the existing height-restricted caveFloor and caveDepth,
with a gradual .30–1.80m mouth transition; roof-top soil remains exposed, wet stone retained.

Separate baseline cave qualification issue: old app also fails scene mean-floor24 (mean23.2619,
spread18.5371,71colour bins). Both full frames were inspected: legible dim roofed tunnel lit through
its mouth, no lighting regression. Mapped ONLY cave-inside to the already-established
`dim-interior-scene` profile (mean20/spread10/40bins, existing numeric thresholds unchanged).
Exterior/overhead cave profiles unchanged, night override retained; routing tests added,11/11
validator and67/67 harness tests pass. This is explicitly a classification correction after baseline
reproduction/manual review, not an image/golden update or a hidden regression. Original failed
scene-profile runs retained: current trial114758.141930Z and baseline115027.563581Z under its
alias captures. They remain recorded as failed runs. Fresh old/new captures now use the same
reviewed profile and executable-content/kernel safeguards. Final eight captures currently running;
performance, final tests, landmark and traversal/holistic ledger rows still incomplete. No completion.


### Qualified — wet litter and covered cave rain (2026-11-02; campaign ACTIVE)

Final composition passes the ORIGINAL storm-ground close-detail gate: std10.19 versus10.0,
baseline9.4213 failed. The earlier10.34 trial was refined so wet response follows only the litter
contribution through later material blends; do not cite that trial as the final binary. Dark humus
and pale leaf/wood retain distinct response, no blanket brightening; default dry ground is EXACTLY
pixel-identical. No new images, samplers, noise, geometry, draws or uniforms. Clear original30%
response on later material contributions/fallback remains. Rejected broad brightening is absent.

Seven final matched pairs including dry control, showers litter, storm stream/clearing, wet beach
trail and storm cave were visually inspected in wet-weather-final.png/.json; full storm ground and
full cave baseline plus cropped ring comparison were inspected.1359cave pixels change: false
rain impacts disappear under the roof, the wet floor/entrance remains. Artifact:
build/campaign-final-comparisons/cave-rain-rings-detail.png. Height-restricted floor mask leaves
upper soil exposed; gradual mouth transition avoids a hard rain boundary. All eight final runs
(two use the same cave comparison profile) passed image/renderer/artifact/kernel gates.
Existing numeric image thresholds unchanged; cave-inside profile classification change is
explicitly explained above, based on the reproduced baseline and reviewed dim tunnel.

Serial warmed private software storm-ground benchmark1280x720/120warmup/600samples:
mean123.4949→122.6125ms(-0.71%),p95142.8147→137.6531ms(-3.61%). Same120–123tracked
submissions,27terrain cells/10draws,127palms,15canopy,1281grass clusters,6–8contact shadows.
User CPU454.48→448.72s, peak process RSS736316→740564KiB(+0.58%). No material regression;
software-only measurements, not hardware speed/VRAM. Full identities/metrics:
build/campaign-wet-weather-performance.json, campaign-performance/wet-weather-{before,after}.
Current app01f681701ffb0b342bf3824e62a1497e3324706adbba0358c7030ad2ad0636b1.
Final build campaign-wet-weather-final-build.log usedCPU0–3/-j3.11validator/67harness tests pass;
final full82-test suite/world report is now running in campaign-final-qualified-* after the final
shader/classification changes. Previous wet-litter full82 passed136.30s. Relevant visual regression
commands are retained in campaign-wet-weather-final-captures.log; no copied GLSL CPU tests added
for these reversible material/effect corrections. The actual compiled/rendered paths are evidence.

Continuing final landmark/vehicle/live walking/cave/stream/flight qualification. Do not call the
goal complete until these and the final original-criteria ledger are reviewed and recorded.


## Final traversal audit reopened — bunker quad defect (ACTIVE, 2026-11-02)

DO NOT complete the goal. All preceding final checks are not closure: the actual last structure
entry sheet revealed a missed functional defect. Existing quad-bunker image gate passed despite
`site open, depth -5.71518, floor17.4266, body24.1712` (quad airborne over6m above the terrain).
Other final CPU82/67harness/11validator tests passed79.77s; final landmarks/quad audit22footprint
rows all0, live cave/stream/flight passed, but they did not test this entry transition. No hiding this
behind an image pass. Exact bad capture: software-captures/20261102T121630.048891Z-quad-bunker.

CPU-only diagnostic build/campaign_bunker_probe_before.cpp reproduces the same launch WITHOUT
obstacles: at32steps depth-5.71639/body24.1736; at24steps ground switches back to hidden
heightfield after losing centre-based bunker classification, injecting a spring impulse. The front
footprints enter the opening before the body centre; switching support solely on quadSite_ is wrong.
Per-query SupportHeight using the existing feet-height hint removes the invisible hill collision
while preserving the actual exposed approach and roof guard. CPU candidate reaches depth2.69017,
body18.5395/floor17.8925 at32steps; diagnostic files/logs in build/campaign-bunker-probe*.

Implementing focused BunkerQuadSupport.hpp (shared production/test floor/normal query), named
BunkerLayout::CorridorFrontDepth (same-.75 value, walking unchanged), replacing only the old
bunker branch of IslandGame::QuadGround. No spring/vehicle engine/landmark/terrain rewrite.
New actual four-wheel32-step regression plus roof/approach guards in BunkerQuadSupportTests.cpp;
new strict quad-bunker protocol rejects the original airborne result and requires entered/settled
floor coordinates.68/68 harness tests now pass. Build currently running onCPU0–3/-j3 in
campaign-bunker-entry-build.log. App BEFORE this fix saved campaign-bunker-entry-before-binary,
SHA01f681701ffb0b342bf3824e62a1497e3324706adbba0358c7030ad2ad0636b1.

Pending: finish build/focused test, actual safe bunker capture and visual/collision review; correct
remaining issues, verify roof/open-driving/other entries, benchmark, rerun final tests (expected83),
update original criterion ledger. Source/helper candidate not yet graphically qualified. Current
quad-bunker camera follows the vehicle, so corrected trajectory changes the camera too: compare
scenario/physical evidence explicitly, do not claim an exact matching-camera pigment comparison.


Bunker continuation: the per-query fix alone admitted entry but a forward-exit CPU probe still
hit the packed heightfield lip (launch>6m). Do not accept the per-query-only screenshot124413 as
final. No spring/drive tuning was used. Narrow cause: the drawn cut/apron ended at-.8/-.9m,
but the coarse mesh interpolates the roof berm into that approach; floor/visible ground differed
~.7m at the portal. Current candidate extends the existing apron to-3m at slope.23, matching the
packed hillside within3cm there. Shared BunkerSite parameters drive layout/support, terrain
cutout via one vec2 uniform, renderer apron and sound's existing outside-floor expression.
Raw authoritative terrain, berm, placements and landmark position remain unchanged.

BunkerRenderer receives the existing read-only RenderedTerrainSurface only for construction,
closes apron side cuts with96soil triangles/one existing batch, buries the structural threshold
and lowers the retained yellow warning strip onto the floor. No new draw/buffer/texture system.
The existing vehicle footprint wall check now covers the actual apron; roof height guard remains.
CPU entry32steps nowdepth9.16787/body-floor.624206; forward exitdepth-4.2555, no overturn,
max transient tyre-plane gaps entry.870819/exit.833298m at full-throttle crest (ballistic suspension,
not a claim that every wheel is always planted).83rd test now covers BOTH directions and15roof
queries. Four focused bunker/cave/rendered-terrain tests pass4.15s. Candidate source built in
campaign-bunker-apron-build.log; graphical apron/side-seam validation, performance and final full
suite still pending. Preserve this active state, do not mark goal complete.

Reverse-only climb on the existing18-degree ramp is rear-drive traction-limited in a CPU probe;
forward exit after turning in a wide room is verified. No new transmission/physics engine or
arbitrary reverse-force boost was introduced to disguise that existing limitation.


Bunker final-source checkpoint: front cut is also closed (32more triangles,128new bank triangles
in total; final bunker7536→7664triangles, same existing two buffers/draw batches, +6144vertex
payload bytes). Roof-head clearance and packed-apron boundary checks added to the real physics
regression. Four focused tests pass4.13s; full83tests pass79.53s,68harness/11validator tests pass,
world fingerprint unchanged. Final capture set campaign-bunker-apron-final-captures.log and
controls campaign-bunker-apron-control-captures.log all pass. Static same-camera comparison and
weather/seam views reviewed: bunker-apron-comparison.png, bunker-weather-final.png. Vehicle
outcome artifact bunker-vehicle-outcome.png is explicitly a same-input/following-camera scenario,
not a same-world-camera pigment comparison. JSON index bunker-apron-final.json stores paths and
controls: quad-drive,quad-cave,quad-mine,quad-audit are all PIXEL IDENTICAL to their previous
qualified images. Original bad bunker log is rejected by the strengthened protocol.
Final measured entrydepth9.16787/body16.4439/floor15.8196/gap.6243m, walls clear. Forward exit
CPU test reaches-4.2555m without overturn/roof collision. Reverse-only uphill limitation remains
explicit (existing rear-drive traction, not silently fixed). Source spring/drive constants unchanged.
Performance now running serially in campaign-performance/bunker-apron-{before,after}, quad-ride
view (riding/active suspension, stationary terrain and same scene), using retained01f... binary
before and current binary after. No competing build/test/render workload. Still ACTIVE.


## Original superprompt acceptance ledger — historical assessment, 2026-11-02

This historical ledger does NOT supersede the ACTIVE status or subsequent user findings. Its
evidence remains useful for scope coverage, but its closure conclusion was invalidated by real
landmark and bunker defects. A fresh full acceptance review is still required. Failed baselines
and rejected experiments remain in the record; compilation alone is not visual qualification.

### Scope, investigation and constraints (original sections1–4,14,16–19,23–24)

The existing application, C++/CNA architecture, authoritative terrain and all landmarks/species
were preserved. README.md, PLAN.md, BUGS.md, visual/incident documentation, galleries, build/test
facilities and actual terrain/environment/vegetation/water/weather/wildlife/player render paths
were inspected. Existing correlated ecology, multiscale water, shadows and animation systems were
reused; no new engine, dependency, survival mechanic or content inventory was introduced. Large
IslandGame.cpp remains748598bytes, CoreTests.cpp913532bytes; focused gait/support/material/rain
helpers and tests live outside them. Current game/tests/tools C++ inventory765files/157048lines;
the prompt's historical counts were not treated as current facts.

User changes present at entry (51tracked+12new files) were preserved. Final state is develop,
8b84844e77e34ac54d4259f397079ff72ec34634,60tracked modifications+17new files. No staging, commits,
reset/clean, merge or push. No CNA/Sharp Runtime source modifications or kernel/firmware/driver
changes. Builds used CPU0–3 affinity and-j3, never more than the user's four-core compilation cap.
Local extraction preserves existing interfaces/ownership; no generated monolithic implementation.
Fixed-seed world fingerprint remains238be1fec0a0cdd7; placement/terrain populations are retained.

AMDGPU GPUVM/flight incidents remain OPEN. Only the documented private Weston/Pixman, isolated
software paths were launched after checking their containment/preflight. Actual runtime renderer
is llvmpipe LLVM19.1.7, Mesa25.0.7, OpenGL33; PCI adapter metadata alone was not mistaken for the
renderer. Kernel monitoring, executable content guards and renderer verification passed. No
physical GPU stress or attempted incident reproduction. Existing BUGS.md CNA-ISLAND-001 Android
surface-lifecycle race and CNA-ISLAND-002 stock-skinned guest/link issue remain external, unchanged,
and unqualified on this final build. No new CNA defect was established or silently patched.

### Environment and presentation criteria (original sections5–12)

Evidence paths below are under build/campaign-final-comparisons unless stated otherwise. JSON
indexes identify exact raw before/after captures. Every listed comparison was visually reviewed;
static contact sheets complement full frames and live motion stages rather than replace them.

| Original criteria | Implemented correction / assessed existing behavior | Evidence and bounded acceptance |
| --- | --- | --- |
|5.1 forest organization;5.2 vertical layers|Kept correlated stand/moisture/drainage/exposure organization and all generated trees. Improved aerial canopy/palm visibility with existing simplified index ranges, rather than adding population. Existing canopy, understory, shrubs, broadleaves and ferns show distinct densities and openings.|aerial.png, forest-review.png/.json, final-build-holistic.png; clearings, coastal/mangrove/stream and interior views reviewed. Distant crowns remain stylized, not photorealistic.|
|5.3 tree forms;5.6 grounding/collision|Corrected mangrove/stilt and canopy foot contact with exact local bank/terrain samples and basal closure; preserved supporting branches/crowns. Existing trunk/crown variants retained where no conspicuous close-view defect was observed.|contact.png, trunk-roots.json and root-contact close-ups; canopy-lod.png and canonical ordinary/mangrove near/far pairs. Root/visibility/terrain tests pass; quad footprint audit22rows zero intersections.|
|5.4 forest floor;5.5 gradual transitions|Shared moisture/canopy response between terrain moss suitability and grass pigment. Wet litter keeps pale organic fragments distinct from dark humus without a new material layer/texture. Existing soil/litter/roots/stones remain spatially selective.|forest-review.png, wet-weather-final.png/.json, final-build-holistic.png. Storm ground deviation9.4213→10.19 passes unchanged10threshold; dry ground pixel-identical. No blanket clutter or uniform brightening.|
|6.1 heightfield;6.2 geology;6.3 soils|Authoritative terrain, ridges, drainage and geological coverage retained. Fixed rock coordinate cancellation by evaluating XY/ZY material charts before blending. Mine approach uses existing earth/rock context and corrected slope lighting.|geology-projection.png, geology-projection-detail.png, geology-projection-near.png plus indexes; six matched east/headland/west/overview/tidepool/contact views. Improvement is local bedding/detail continuity, not a terrain regeneration.|
|6.4 rocks/roots;6.5 caves;6.6 distances|Land/reef rock basal grounding and tree contacts corrected. Existing cave roof/soil/vegetation geometry retained after overhead/mouth/interior inspection; player support/capsule sweep corrected. Bunker apron now joins actual packed terrain.|Rock/root stage indexes and tests; landmark-final-review.png/.json, final-build-live-review.png, bunker-apron-comparison.png. Near, coast, elevated and real flight views inspected; no open cave roof seam observed.|
|7.1 waves;7.4 sheltered/exposed water|Existing directional multiscale/warped wave model retained after investigation. Reduced rigid caustic grid through existing warp components and corrected foam pigment relative to its opacity. Rejected speculative depth/flat-wave experiments were removed.|water-review.png/.json, original synchronized reef/water pairs. Coast/current/lagoon/exposed views reviewed; no costly new fluid simulation. Foreground fixed-view rings were traced to intentional player disturbance, not claimed as a CNA defect.|
|7.2 shallows;7.3 shoreline/surf|Preserved depth absorption, seabed readability, nearshore composition and existing shoreline fields. Foam correction makes disturbances read as foam without relocating surf onto land.|water-review.png, wet-sand/current/wreck wide frames, final-build-holistic.png. Sand, rocks and deep/clear-water transitions remain legible; no hard new plane/material boundary observed.|
|7.5 underwater habitat;7.6 tides;7.7 exploration|Retained habitat masks/fish depth rules; reef props seated by existing grounding fixes. Tide-relative diving/telemetry and strict surface/dive/resurface protocol corrected.|reef-habitat/fish images, software-fullscreen/20261102T133945.409145Z, final-build-live-review.png; CPU tide/placement tests pass. No claim of a complete12minute visually recorded tide cycle or continuous visual fish-motion qualification.|
|8.1 anatomy;8.2 grounded gait;8.3 weight|Bear face/fur geometry corrected, existing coat/atlas retained. Boar cadence/stride and deer phase follow accepted surface travel and scale; deer body/hoof support follows terrain and stance/swing in visible/shadow passes. Existing goat foot system retained.|wildlife-review.png, wildlife-motion-review.png, deer-gait-walking.png, deer-gait-settling.png, deer-gait-sequence.gif, earlier boar/deer support indexes. Live dry/rain walk/stop, CPU slopes/extremes and static controls qualified. Not fixed-length IK or persistent world-foot locking; turns/acceleration may slip briefly.|
|8.4 idles;8.5 interaction;8.6 habitats/density|Stopped deer/boar no longer continue travel-based stepping. Deterministic turtle nesting recovery validates actual footprint/capsule and habitat rather than forcing nests through obstacles. Existing bird/fish/crocodile/turtle/monkey behaviors and habitat preferences inspected and retained.|Live stop stages, standing/bedded controls, wildlife/reef review; population/navigation/turtle tests. No new species or population inflation. Aquatic/bird static frames are representative habitat/anatomy evidence, not a full continuous-motion audit of every species.|
|9.1–9.5 resident proportions, silhouette, material, animation and scope|Corrected sleeve attachment, taper-aware normals, oval belt/waist transition while preserving person/rig/route/material identity. Existing day/tending/fishing and supported evening pose retained.|resident.png and resident waist indexes; resident-motion-final-review.png, lighting-review.png, final8stage resident capture20261102T133922.797345Z. Near mesh7142→6814triangles, two draws retained. No avatar-engine or photorealistic remodel.|
|10.1 art direction;10.2 lighting;10.4 materials|Restrained soil/grass/litter/bark/skin/cloth/water changes preserve stylized island palette. Corrected mine slope shading and wet organic absorption. Existing sky/day/weather lighting retained after actual clear/dawn/overcast/rain/night/interior review.|lighting-review.png/.json, wet-weather-final.png, resident and geology comparisons; final-build-holistic.png. Cave stays intentionally dim; only cave-inside routed to existing dim-interior profile after old/new baseline review, no numeric threshold weakened.|
|10.3 shadows;10.5 LOD|Reused SunShadowMap with aerial focus correction, synchronized root/animal support in shadow pass. Aerial geometry uses existing vertex/index buffers and one instance upload with binding offsets; ordinary near/ground policies preserved.|Aerial shadow-focus/LOD comparisons and live flight; SunShadowFocus/CanopyVisibility tests. Existing CNA buffer-binding guard respected; no AO/new shadow engine or disabled LOD. Stock fallback retains behavior but current visual qualification is custom OpenGL33 only.|
|10.6 weather coherence|Finite shelter clipping/finer eaves and canopy drips corrected. Covered cave puddle rain rings suppressed using existing cave depth/floor while roof soil stays exposed. Wet leaf fragments retain material contrast.|Rain shelter/drip comparisons, wet-weather-final.png, cave-rain-rings-detail.png, bunker-weather-final.png. Storm hut/cave and day/rain/night bunker checked. Wet stone retained; no emissive brightening or weather engine replacement.|
|11.1–11.4 landmark separation, overcrowding, grounding and character|All landmarks retained and inspected through approaches/sightlines. Existing vegetation/negative space retained; mine earth treatment and bunker portal support/cut/apron match surroundings. Wreck/cave/hut/lighthouse/helicopter/volcano preserved.|landmark-final-review.png/.json, water wreck view, bunker-apron-comparison.png, bunker-weather-final.png, final overview. No relocation/addition of attractions; existing terrain and vegetation provide partial reveals without hiding every distant landmark.|
|12.1 walking;12.2 structures|Bounded player substeps and speed-aware downhill contact fix observed stream bank lock/hover. Cave supported-feet sweep fixes capsule penetration. Bunker player floor/walkable cut shares named apron parameters.|Existing RED→GREEN stream/cave regressions; final live cave/stream logs and images, BunkerLayoutPlayerMotion tests. Final cave/stream actual keyboard scenarios pass. No slope-threshold or collision architecture replacement.|
|12.3 swimming;12.4 vehicles;12.5 regression|Final diving and flight/landing pass. Bunker query now supports front wheel footprints before body centre enters, with height hint preserving roof safety; existing apron extended/closed against packed terrain. Strengthened actual-entry guard rejects old airborne screenshot.|BunkerQuadSupport real four-wheel32step entry/forward-exit test,15roof guards and apron boundary checks; finalquad-bunker depth9.16787/body-floor.6243m/walls clear. Quad drive/cave/mine/audit controls pixel-identical. Existing reverse-only uphill traction limit remains; no force hack, no claim of visual Uturn/reverse qualification.|

### Validation, performance and evidence integrity (original sections13–15,20–22)

Release/OpenGL33/Ninja/GCC14.2 final app SHA256:
`6e06335fa7eb034831bddf08eb4092fd6e0957371c7ec9d70048a2c64cd8b6ca`.
83/83 safe CTests passed79.53s before the final retention-only Python change.70/70 capture-harness
and11/11 image-validator tests pass after it; both corresponding CTest targets also pass. Final
full83test rerun is recorded separately below once finished. World fingerprint238be1fec0a0cdd7.

Commands actually executed (graphics only through the documented safe supervisors):

```sh
taskset --cpu-list 0,1,2,3 env CCACHE_DIR="$PWD/build/ccache" cmake --build build -j3
ctest --test-dir build -LE display -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2
python3 tests/SoftwareCaptureTests.py
python3 tests/ValidateCaptureTests.py
build/island-world-report --output build/campaign-bunker-apron-final-world.json
python3 tools/run_software_capture.py build <documented deterministic viewpoint>
python3 tools/run_fullscreen_software.py build --diving  # also --cave, --stream-crossing, --flight
```

36 display-tagged CTests and2orphan validators were intentionally excluded to avoid uncontrolled
GPU launches/missing producer artifacts; they are not reported as passed. Approved software
captures replace that display path. No Debug/Web/Android/alternate backend/native physical GPU
validation is implied. Software-process RSS is not VRAM; reported timings are not native FPS.

Same resolution1280x720, VSync off, LP8,120warmup/600samples, serial same-view software pairs:

| Isolated change | Mean ms before→after | p95 before→after | Complexity / interpretation |
| --- | --- | --- | --- |
|Rock projection|82.6978→83.2917(+.72%)|92.2332→92.8125(+.63%)|Same70–72tracked draws/8terrain draws; RSS831784→827212KiB.|
|Live deer gait|161.7113→158.2051(−2.17%)|176.7515→177.3941(+.36%)|Same108–111draws; RSS800120→795196KiB.|
|Wet litter/cave rain|123.4949→122.6125(−.71%)|142.8147→137.6531(−3.61%)|Same120–123draws; RSS736316→740564KiB(+.58%).|
|Bunker active suspension, reverse-order repeat|85.9732→85.5408(−.50%)|100.3043→100.5235(+.22%)|Same97–98draws; RSS777584→777388KiB. Initial96.6624→85.6206 baseline was an outlier, retained; no11%speedup claim.|

Raw identities/CPU/RSS/scenes in build/campaign-geology-performance.json,
build/campaign-deer-gait-performance.json, build/campaign-wet-weather-performance.json and
build/campaign-bunker-apron-performance.json. Earlier aerial visibility cost+6.45% mean/+6.66%p95
for42→192visible canopies is explicitly justified above with cheaper existing index ranges;
raw high-cost expansion and poor silhouettes were revised. Existing palm/root/rain/shadow/rock/
player stage comparisons and variance investigations remain in their historical sections. No
material unexplained performance regression was accepted. No cumulative speedup is inferred by
adding unrelated views or by comparing different final feature sets. Loading buffers/resources
were checked locally: final bunker adds128triangles/6144vertex bytes/one vec2uniform, no new draw
or buffer batch. Resident mesh shrank; tree population/static vertex payload did not increase.

Visual review used deterministic camera/seed/time/weather/resolution where changes required it.
Before/after raw sources were retained and restored; no temporary diagnostic or benchmark-only
source remains. Following-camera vehicle outcome is labeled as same-input scenario, not falsely
as an identical camera comparison. Pixel-identical controls and exact clocks complement actual
image inspection; no golden updates were used as proof. Rejected wet brightening and failed
storm/cave old-profile captures remain failed; bunker121630image-only false positive is superseded
and rejected by the final functional validator, never counted as a functional pass.

Final exact-build overview/jungle/reef/resident/storm views:
build/campaign-final-build-holistic.log and build/campaign-final-comparisons/final-build-holistic.png.
Final exact-build keyboard diving/cave/stream/flight:
build/campaign-final-build-live-input.log and
build/campaign-final-comparisons/final-build-live-review.png, with raw paths in
build/campaign-final-comparisons/final-build-live-review.json.
These final sheets were visually inspected: forest ground/crowns, shallow reef, resident silhouette,
storm substrate, cave mouth/roof, bank crossing and aerial return/landing remain coherent. No new
conspicuous clipping/seam/terrain-contact regression was observed in those tested views. The island
remains stylized, with deliberate open slopes/clearings and existing unusual landmarks; further
polygon/population/engine changes would exceed the observed-defect polish scope.

Machine-readable final audit: build/campaign-final-evidence-audit.json. It checks97run directories
(89fixed-view,8native-input), runtime llvmpipe, private Pixman wrapper, unchanged launch/exit
executable hashes, image gates where applicable and zero new kernel GPU faults. One old bunker
run is explicitly a superseded functional false positive.319timestamped raw paths cited by the
comparison indexes exist (build/campaign-final-index-path-check.json). Earlier stage indexes and
298unique timestamp citations are included for retention/provenance, not automatically declared
new final-build runs. Human visual judgments are recorded above, not inferred by this script.

Retention bug fixed: tools/capture_retention.py now honors uppercase HANDOFF.md and legacy
handoff.md, plus directly cited bounded JSON indexes inside build; it rejects escaped/symlinked
outside paths and invalid/oversized reads. Two functional tests exercise preservation and safe
pruning. The audit index above is explicitly cited so its raw before/after evidence remains
protected, without duplicating handoff files or recursively scanning arbitrary files.

Meaningful remaining limits: unresolved physical AMDGPU qualification; untested current Debug,
Web/Android/stock-fallback/other backends and existing documented external Android issues;
procedural animal turn/acceleration foot slip; rear-drive reverse bunker climb traction; no full
visual tide cycle or exhaustive continuous motion capture of every aquatic/bird species. These
are explicit qualification/simulation bounds, not claims that this campaign repaired external
drivers or every pre-existing behavior. Major observed defects were fixed and qualified; no
additional high-value safe in-scope correction remains identified by the final holistic review.


### Final closure record

Final full safe suite after the retention fix: **83/83 PASS,73.36s**, including the expanded70test
capture harness and11test validator; build/campaign-final-acceptance-tests.log. Direct Python logs:
build/campaign-final-acceptance-harness.log and build/campaign-final-acceptance-validator.log.
Final CPU0–3/-j3 build reports `ninja: no work to do`. No C++ mutation after the graphically
qualified final executable. `git diff --check` passes. Repository/source sizes and exact identity:
build/campaign-final-repository-state.json. Audit retention check protects all298index stamps
(625total handoff/index stamps); no capture pruning was run manually. Comparison sheets67MiB,
software-captures882MiB, benchmark evidence681MiB; no evidence was deleted to manufacture cleanup.

Original stopping criteria21 were assessed: major observed visual/grounding/gait/weather/collision
issues are addressed; reviewed matched comparisons demonstrate the intended changes; safe tests
pass; no serious new regression is known; controlled software costs are investigated; further
expansion would offer diminishing returns or require separately scoped hardware/platform work.
Section22deliverables are implemented source/tests, reviewed images and indexes, exact commands/
results, measured performance, meaningful limits and uncommitted repository state in this file.
This closure records the complete bounded campaign, not universal hardware/platform qualification.


## Reopened continuation — aquatic and flying wildlife motion, 2026-11-02 (ACTIVE)

User requested further work against the entire original campaign. Existing source, attachment,
status and incident/safe-render instructions rechecked;60tracked+17new files preserved on develop.
The previous assessment explicitly lacked actual motion sequences for fish/birds. Next step:
add two small named motion views to the existing fixed-clock/capture path, retaining the ordinary
fish/birds anatomy views and AI; inspect real moving subjects before deciding any model/movement
correction. No external source/hardware/system work. CPU0–3/-j3 continues. Do not close at this
instrumentation milestone; qualify observed defects and revisit the remaining criteria afterward.

First new fish-swim run20261102T143642.413166Z exited139 because the new telemetry incorrectly
dereferenced the lazily allocated player swimming surface. This is an instrumentation mistake,
not a pre-existing FishSchools defect. Switched to the already-created drawnGround_ surface
including stream/wreck patches. Failed evidence remains failed; llvmpipe/Pixman was verified and
kernel delta is retained. Baseline binary b5b9f373... contains that invalid telemetry and must not
be used as an accepted benchmark or motion baseline. Rebuild/corrected captures next.

Corrected motion instrumentation baseline ec653280ae07a4e8351d03ecaca0fd940da4e98e29f30acb9bf1772c8e2d3b31
saved as build/campaign-bird-feather-before-binary. Fish sequence143811passes16ordered stages,
5.7418m travel and1.9602body-wave range, all17images validated. This close camera provokes the
existing swimmer avoidance, so a new fish-swim-only distant vertical observer will separately
qualify ambient schooling; ordinary fish gameplay/view unchanged. CPU3600frame probe has31302
Full-tier samples,0estimated buried bellies, min-gap.06616m. It does not test every mesh vertex.

Bird first sequence143858was safely rendered and inspected but failed near-count telemetry:
BirdRenderer is reused for flock then booby/weaver colonies, resetting its public counts each
draw. New motion proof now snapshots the flock count immediately after its actual draw; no
loosening of the near-tier requirement. Corrected144355sequence passes and shows actual upstroke
vanes collapsing into long needle-like strips. Root cause in AppendFeatheredWing: body-up feather
planes do not follow lifting arm/hand spans. Current candidate derives separate span normals,
projects vane widths and lighting into their support planes, same population/vertex/draw budget.
BirdWingNormal helper and bank/fold/flap CPU regression added. Baseline controls still running;
candidate not built/rendered/accepted yet. Continue qualification, not a milestone stop.

Feather candidate144756rendered and all16stages/full frames reviewed against144355: raised
secondaries/primaries now remain connected broad vanes instead of needle-like combs; downstroke
shows coherent blue flight feathers. Subject/time/route preserved. Same mesh calls/vertex budget.
3focused tests pass, including288bank/fold/lift/side normal fixtures. Baseline bird-preen144540
failed the pre-existing proof for the same overwritten colony counters, despite a rendered
perched subject. Keep failed status. Narrow correction: render primary flock last (same established
policy as fish schools), so all existing overlay/startup/preen counters refer to the advertised
24bird population. Actual colony/flock batches retained; this is not an increase in population.
Pending final rebuild, folded/soaring/ambient fish and rain controls, software cost, full suite.

Bird continuation qualified visuals (current app before tide instrumentation): final11captures
in build/campaign-bird-feather-final-captures.log all pass, including sixteen-stage dry/rain
flight, preening, static parrot/ground dove/frigatebird, ambient fish, static fish, overview/jungle.
Reviewed bird-feather-final.png, bird-controls-final.png, fish-swim-final.png and full upstroke.
Index build/campaign-final-comparisons/bird-feather-final.json has exact paths, state equality and
controls. Bird route/heading/bank/wing values match across dry before/after exactly;28.3803m
travel,1.8297wing-pose range. Ambient fish2.95728m/body-wave1.94899; prior5.74m run was actual
close-observer evasion, not ambient. Static fish/overview/jungle are PIXEL IDENTICAL.
288CPU wing frames pass; old body-up implementation injected through temporary include fails
with "Feathers do not follow the lifting span" (build/campaign-bird-feather-red.json). Production
source untouched by that RED diagnostic.84/84safe tests pass129.59s (concurrent capture affected
timing; do not treat suite duration as performance). World fingerprint unchanged. Serial isolated
software benchmark is now running, without concurrent build/test/capture work.

Next unfinished criterion7.6: high/low tide visual integration. A capture-only held phase setter
and strict first/final-frame sea-level proof are implemented but NOT built/accepted yet. Default
tide clock/gameplay untouched; same TideCycle::At model, no new simulation. Continue with build,
CLI/preflight tests and paired beach/wreck/mangrove/shallows views at phases.25/.75 after the
benchmark finishes. Do not overwrite a timed executable during the benchmark.

Completed serial LP8 software bird benchmark (static birds view,1280x720,VSync off,120warmup/600samples):
mean138.1332→136.4837ms(-1.19%),p95149.2481→149.4615(+.14%); CPU360.33→359.09s,
RSS705444→708992KiB(+.50%). No material regression or speedup claim. Full identities/data:
build/campaign-bird-feather-performance.json; both actual binaries/runtime unchanged through runs.
The bird population metric now correctly refers to the primary flock, not the weaver colony;
this diagnostic correction is not a population increase. All original batches/vertex budgets retained.

Tide-phase build now running in build/campaign-tide-phase-build.log onCPU0–3/-j3, after benchmark
finished.72/72harness tests pass, including phase bounds and strict first/final-frame proof. CLI
invalid/range/mode tests added; awaiting compiled binary tests, high/low captures and inspection.
No completion at this milestone; continue next unfinished tide acceptance criterion.

First held-tide run150707exited2 before graphics: the new CLI guard rejected smokeTest, but the
existing --capture parser intentionally sets that flag itself. Removed that incorrect condition;
fixed captures remain required, benchmark/startup/inspection rejected. CLI case now verifies the
actual forbidden startup-report combination (still38invalid invocations), rather than a valid
smoke+capture pair. This failed run stays failed, not rendered/tide-qualified. Main-only rebuild
next, followed by one successful held-tide capture before the rest of the matrix.

## Continuation final evidence and original-criterion reassessment — 2026-11-02

The reopened campaign proceeded past the bird milestone into the remaining tidal integration
qualification. This section supersedes pending continuation notes and the earlier closure counts.
The full original criterion-by-criterion ledger above is retained; the following evidence closes
its representative bird/fish motion and high/low coastal-state gaps without inventing new content.

### Implemented / actually reviewed

- Bird flight feathers now use separate supporting arm/hand span normals for vane width,
  direction and lighting. Upstroke vanes remain coherent surfaces instead of needle-like strips;
  downstroke retains a readable fan. Original species, AI routes, body/head/tail, vertex counts,
  buffers and batches remain. The primary flock renders last, matching the existing fish-school
  policy, so overlay/startup/preen counts describe that flock instead of the final colony.
- Added only named `bird-flight`/`fish-swim` inspections to the existing fixed-clock path, with
  translation-following camera and sixteen stages every.2s through3.2s. Strict CPU validation
  requires ordered clock, Full-tier near rendering, finite/habitat-bounded poses, real travel,
  animation variation and bounded heading/position steps; each stage image is separately gated.
  Ambient fish inspection keeps the QA observer outside swimmer-parting distance. Ordinary fish
  simulation/appearance is unchanged; the earlier close-camera evasion sequence remains separate.
- Added capture-only `--tide-phase` support in the existing supervisor, forwarding
  `--capture-tide-phase` to the game. It holds the existing TideCycle clock only; default play
  keeps its advancing clock. Range/mode validation happens before graphics; first/final-frame
  phase and sea-level proofs prevent a mislabeled snapshot. No tide/water/terrain simulation rewrite.
- Reviewed matched dry/rain flight, static parrot, folded preening/ground dove and soaring
  frigatebird comparisons. Exact route/heading/bank/wing states match through all dry stages.
  Primary flight28.3803m/wing-range1.8297; ambient fish2.95728m/body-wave-range1.94899. The earlier
  close-observer evasion sequence travels5.7418m; it is not represented as ambient schooling.
- Reviewed six matched high/low coastal states (+.180000007/-.180000007m): wet sand, actual sandy
  bay, mangroves, shipwreck approach, submerged reef and retained tide pools. Sand exposure and
  waterline change coherently; roots, wreck and reef remain grounded. Low-water fish motion also
  passes all16stages and was reviewed. The old `beach` view frames a forest approach here, so its
  extra pair is a negative control; it was not used to claim a visible shoreline comparison.
- A suspected floating bay log was investigated before changing it. CPU probe of all34main-body
  meshes against rendered terrain shows all visible bay pieces touch/penetrate sand by about
  .003–.061m; raised ends account for the appearance. Only one remote piece has a1.50cm minimum
  gap. No indiscriminate sinking/remeshing was applied. Diagnostic evidence:
  build/campaign-driftwood-probe-before.log. This tests main-body vertices, not every branch face.

Actual reviewed artifacts/indexes (all under build/campaign-final-comparisons):
`bird-feather-final.png`, `bird-feather-sequence.gif` (finite3.2s recording, not seamless motion),
`bird-controls-final.png`, `fish-swim-final.png`, `bird-feather-final.json`,
`tide-extremes.png`, `tide-extremes.json`. Explicit retention index references:
build/campaign-final-comparisons/bird-feather-final.json and
build/campaign-final-comparisons/tide-extremes.json.
The bird before/after comparison is a genuine geometry correction; tide state pairs and fish
sequences are qualification evidence, not claims of a newly implemented water/fish renderer.

### Validation and comparable software cost

Final Release/OpenGL33 app SHA256
`66234fb26194a6c1b603cf6a756e1fcf7c6ae53370234504d4de6f8bbb4252c9`.
CPU0–3/-j3 build current (`ninja: no work to do`). Full safe CTest suite **84/84 PASS,81.70s**:
build/campaign-tide-extremes-full-tests.log. After the final stricter fish-bank proof check,
4/4 focused CTests pass in1.30s (build/campaign-continuation-final-focused.log), direct
**72/72 capture-harness and11/11 image-validator tests pass** in
build/campaign-continuation-harness-final.log and build/campaign-continuation-validator-final.log.
CLI verifies38invalid invocations; its CMake expected count was corrected from26, not loosened.
New BirdWingNormal regression covers288bank/fold/flap/side poses. The old fixed-body-up frame,
injected through an owned temporary include directory, fails the same regression; production
source was never left with that stub (build/campaign-bird-feather-red.json).
World fingerprint remains238be1fec0a0cdd7 (build/campaign-tide-extremes-world.json).

Test/build/capture commands remain those in the original ledger, plus:

```sh
ctest --test-dir build -R '^(IslandSoftwareCapturePreflightTests|IslandCaptureValidatorTests|IslandCommandLineValidation|IslandBirdWingNormalTests)$' --output-on-failure
python3 tools/run_software_capture.py build bird-flight
python3 tools/run_software_capture.py build fish-swim
python3 tools/run_software_capture.py build wet-sand --tide-phase .25  # .75 for low water
```

Software-only serial birds benchmark,1280x720/VSync off/LP8,120warmup+600samples:
mean138.1332→136.4837ms(-1.19%),p95149.2481→149.4615ms(+.14%),CPU360.33→359.09s,
RSS705444→708992KiB(+.50%). No material regression or meaningful speedup claim. Same terrain
14cells/3draws,106palms,2canopies,1010grass clusters. Tracked submissions102–103→103–104 because
public bird counters now retain the visible primary draw instead of an empty final colony;
this is a repaired counter, not an added actual bird draw. All original draw batches remain.
Full data/identities: build/campaign-bird-feather-performance.json. Timing binaries precede only
the subsequent capture-only tide instrumentation; no additional water/geometry cost is claimed.
No build/test/capture competed during the measurements. Test-suite durations are not benchmarks.

Default fish, overview and jungle controls after the feather correction are pixel-identical to
matching baselines. After tide instrumentation, overview and the final flight image remain
pixel-identical, and all16default flight-stage PNGs are byte-identical to the qualified pre-tide
build. This complements visual review and the clock proof; it does not replace them.

Machine audit: build/campaign-continuation-evidence-audit.json checks35successful capture sets,
179image validation results, runtime llvmpipe, private Pixman wrapper, exact artifact before/after
content and zero new kernel GPU faults. The4failed instrumentation/old-proof/CLI runs remain
explicitly failed and retained there. Earlier97run audit remains separate historical evidence;
these are not97new final-build runs. Software llvmpipe LLVM19.1.7/Mesa25.0.7 is actually logged,
not inferred from an environment variable or PCI adapter label. No physical GPU workload.
36display CTests and2orphan validators remain intentionally excluded, not claimed passed.

### Original criteria and final stopping assessment

| Original area | Continuation disposition |
| --- | --- |
|1–4constraints/investigation/safety|Rechecked attachment/status/incident instructions; no external/system/hardware edits, no lost changes, at most4compilation cores.|
|5forest and6terrain/geology/grounding|All earlier fixes/evidence retained; default jungle/overview exact controls pass. New beach-log suspicion was measured and rejected, preserving valid grounding.|
|7.1–7.5waves/shallow/coast/habitat;7.6tides;7.7underwater|Existing improvements retained; actual high/low wet-sand/bay/mangrove/wreck/reef/pool pairs and low-water fish reviewed. No new seam/floating artifact observed. No claim of a recorded continuous12minute cycle.|
|8wildlife anatomy/movement/idle/habitat|Previously missing representative fish/flight sequences now exist; actual bird vane defect fixed and matched dry/rain/folded/soaring controls pass. Earlier bear/boar/deer/goat/monkey/turtle evidence retained. Not exhaustive continuous capture of every species.|
|9resident;10lighting/shadows/material/LOD/weather|Earlier implementations/qualified controls retained; feather lighting now follows wing planes, day/rain/fold/soar views remain coherent. No new lighting/shadow architecture.|
|11landmarks;12traversal/collision|Existing cave/mine/bunker/stream/diving/flight fixes and previous exact-build live evidence retained; wreck/tide approaches checked. No unsupported claim of fixing rear-drive reverse traction or demonstrating a vehicle Uturn.|
|13performance;14maintainability/determinism|Measured isolated cost with explicit counter correction; no new mesh budget/resources/dependency. Small helper/test outside giant files; fixed world fingerprint unchanged.|
|15visual QA;16–18execution/quality|Real baseline→implementation→build→render→inspect→test loops completed, followed by the next unfinished tide priority and a measured prop hypothesis, not stopped at a milestone. Failed attempts never relabeled as passed.|
|19known issues;20holistic review;21stopping;22–24delivery/principles|Original full ledger plus continuation evidence recorded. No further high-value safe in-scope defect identified in final review; no new feature inventory or endless cosmetic churn. Remaining hardware/platform/simulation bounds below are explicit.|

Remaining meaningful limits are unchanged: AMDGPU GPUVM/flight incidents OPEN; no current native
physical GPU/Debug/Web/Android/stock-fallback/other-backend qualification; existing external Android
issues remain; procedural land-animal acceleration/turn slip and rear-drive reverse bunker-climb
traction remain bounded simulation limits. High/low states and representative3.2s sequences are
not exhaustive all-tide/all-species temporal validation. No zero-defect or photorealism claim.

Current repository: develop,8b84844e77e34ac54d4259f397079ff72ec34634,63tracked modifications+19new
files, all uncommitted; entry60tracked+17new preserved. No staging/commit/push/merge. Current C++
inventory767files/157202lines; IslandGame.cpp752268bytes, CoreTests.cpp913532bytes. Exact state:
build/campaign-continuation-repository-state.json. No pending build/test/render process. All
observed continuation defects are corrected and qualified; the original criteria have again been
assessed with the new evidence before deciding completion.


Final recovery check: continuation audit identity and indexed raw paths verified; retention covers
all new audit stamps,664total. `git diff --check` passes, staging is empty, final build has no work.
Original stopping criteria were applied after the complete original ledger and continuation
reassessment were written, not after the bird milestone. No further observed high-value safe
in-scope correction remains identified; unverified hardware/platform and explicit simulation
bounds remain listed rather than being represented as passed.


## New active continuation — alarm recovery and turning, 2026-11-02

The user explicitly rejected repeated stage-based closure. No completion call is justified by
one corrected subsystem. Initial read-only investigation found the bedded-alert mismatch and
stationary heading updates above. Baseline reproduction/captures next; all earlier work preserved.


## User-directed checkpoint commit/push and reported audio regression — ACTIVE

User explicitly authorized commit and push on develop. Saving the validated accumulated campaign
now; new deer-alert work is diagnostic only under ignored build (baseline confirms bed=1 while
heading changes90degrees, speed/travel/phase0). No unqualified deer fix is included yet.

User reports lost sound; this is now the first investigation priority. Their latest actual native
interactive log1881892 shows audio enabled, Soundscape started and4–5playing sources, but this
DOES NOT establish audible output. Inspect actual backend/device/routing and PCM before claiming
any fix. Do not launch AMD graphics or change host audio settings without a demonstrated need.

First required remote fetch failed SSH authentication (publickey; ssh-askpass absent), not a
merge conflict or auto-review rejection. Commit is independent; inspect existing SSH-agent/GitHub
authentication for a safe authorized push, never print private key/token contents. Keep campaign
active; commit/push and sound investigation do not replace the original objective.

### Checkpoint pushed; sound output investigated, 2026-11-02 — ACTIVE

Signed ad48d3d4 (82files,5273insertions/498deletions) pushed normally to origin/develop;
upstream configured. The default SSH agent held only an old read-only GitHub account key.
Using the existing unlocked matching key through SSH_AUTH_SOCK=/run/user/1000/keyring/ssh
solved authentication; no keys/configuration/remote were changed and no force push was used.

The lost-sound cause is NOT established and no playback fix is claimed. The user's native
interactive run1881892 was unmuted with4–5sources, but did not log the selected audio driver.
Host default Speaker sink is unmuted54%; saved SDL Application stream properties unmuted100%.
Audio-only probes of CNA and actual Soundscape generated nonzero PCM via PulseAudio to that
sink (build/campaign-audio-native-probe.log, build/campaign-soundscape-native-probe.log and
corresponding routing JSON). These are not proof of physically heard sound or the failed run.

Actual whole-game test, unchanged entry binary, automatically selected pulseaudio:
build/software-audio-output/20261102T162431.733299Z/{run.json,routing.json,pcm-analysis.json,
interactive.png,own-game-audio.wav,game.log,kernel-live.jsonl,kernel-final.json}.
Private /dev had no /dev/dri; game reported llvmpipe, private Weston used Pixman, mandatory
kernel supervision found no new GPU fault, executable content unchanged. Only this game's
S16stereo44100 device input was recorded, no microphone/speaker monitor/other applications.
Own stream targeted Speaker sink65, unmuted100%, not corked; stream writes had0errors.
Last1second windows: enabled RMS.057862 (−24.752dBFS), M muted exact0, resumed RMS.058364
(−24.677dBFS). Actual screenshot inspected; normal beach/jungle scene present. This proves
non-silent device-bound PCM and mute recovery in software rendering, NOT native-GPU/audio
qualification or physical audibility. Three earlier diagnostic runs failed harness setup/format/
input handling and are explicitly failed in their run.json; none counted as passed.

Added a bounded startup audio-runtime diagnostic in the existing RuntimeDiagnostics module,
called after actual successful Soundscape startup. It logs selected SDL driver and both driver
override names, explicitly labels dummy/disk non-audible. Existing native log could not
separate a silent inherited test driver from normal playback; no speculative volume/backend
change made. Regression/build and rebuilt whole-game confirmation pending. User question about
sound in other applications remains unanswered; continue independent campaign work meanwhile.

Audio diagnostic implementation qualified: Release/OpenGL33 build passed with CPU0–3/-j3;
IslandSoundscapeRuntimeTests passed3.71s, including persistent-log assertions that actual dummy
is identified as non-audible. Whole-game rebuilt confirmation:
build/software-audio-output/20261102T163051.852136Z/{run.json,game.log,routing.json,
pcm-analysis.json,interactive.png,stages.json} passed, explicitly reports pulseaudio with both
overrides unset, writes nonzero PCM, exact silent mute and resumed PCM,0stream errors.
Native Wayland software path also passed on the entry binary:
build/software-audio-output/20261102T162937.206404Z/{run.json,game.log,routing.json,
pcm-analysis.json}; llvmpipe/Wayland, PulseAudio, RMS.049116(−26.175dBFS),0stream errors,
same Speaker routing, artifact unchanged, no new kernel faults. Build processes were active
outside that diagnostic; no performance inference is drawn. Shells in this project currently
show no inherited SDL audio overrides; this does not recover the ended failed process's env.
No host audio setting or dependency changed. Missing-sound report remains unresolved pending
reproduction/user confirmation of audible output and sound in other applications. Continue
with deer alarm recovery independently; do not mark the overall campaign complete.

### Deer alarm recovery — implementation and visual qualification in progress

Audio diagnosis checkpoint5f5f14b4 pushed to origin/develop. No lost-sound fix claimed.
Confirmed bedded-deer bug with a RED regression (build/campaign-deer-alarm-red.log:
Bedded deer pivots or slides before rising) and inspected16-stage baseline:
build/software-captures/20261102T163625.737056Z-deer-alert.
Old bed remained1, speed/travel/phase0 while heading changed90degrees.

Focused existing-DeerHerd correction now raises an alarmed/startled resting deer first,
permits body turns during locomotion, uses a slow bounded alert shuffle through accepted
travel/gait, then stands watching. Rejected alert steps restore heading/speed and cannot
advance gait. Standing stamps wait until the shuffle finishes. No new rig, mesh, population,
shader, material or environmental generation was added. DeerHerdTests now covers left/right
observer and noise,60Hz/.1s steps, rise-before-turn, actual stepping/settling, habitat/ground,
and blocked shuffle; existing hoof/gait regressions pass (3/3,1.24s).

First corrected development capture20261102T164003.585054Z-deer-alert inspected: rises,
steps and stops instead of swivelling folded legs. It followed animal translation with the
camera; superseded for final before/after comparison. The final new deer-alert QA camera is
now fixed in world space to match the baseline exactly; rebuilding/recapture pending. Existing
deer/deer-walk/deer-bedded camera behavior retained. New QA view uses existing capture clock,
16stages at frames1,5,...61 and a fixed16m lateral alarm observer; ordinary gameplay unchanged
by QA scaffolding. No completion claim; sound reproduction and actual bunker maneuver remain.

User confirmed sound works and explicitly instructed not to investigate audio further
("uz je to vyreseno, zvuk neres, fugnuje"). Sound incident closed by user confirmation;
no root cause or playback-code fix is claimed. Keep the already qualified diagnostic commit.
Continue original campaign with deer qualification and actual driven bunker maneuver.

Deer final qualification update: fixed-camera final set
build/software-captures/20261102T165517.788627Z-deer-alert is pixel-identical in all16stages
to20261102T164238.047476Z-deer-alert. Reviewed comparison:
build/campaign-final-comparisons/deer-alarm-recovery.png/.gif/.json. Same fixed world camera,
seed,1280x720,14:00/partly-cloudy and .1s clock. Ordinary walk and bed controls passed and were
inspected at20261102T164314.021788Z-deer-walk /20261102T164343.662647Z-deer-bedded.

Full available suite initially83/84passed250.42s; CloseObserverTurning correctly caught a
boundary regression: suppressing a blocked shuffle's heading left the animal facing away.
Corrected by retaining the old stationary-facing fallback when a shuffle cannot fit (no fake
travel/gait). Focused DeerHerd+CloseObserverTurning rerun2/2passed2.89s; full rerun pending.
Earlier text saying blocked steps always restore heading is superseded by this correction.
Ordinary open-ground alarms rise and step; no claim of persistent foot locking for blocked
turns/acceleration. Model/vertex/draw budgets unchanged (2draws in matched alarm sets).
Capture timing varied broadly with host load (grass95.71→147.33ms, deer.15→.23ms); not a
controlled performance comparison and no speedup claimed. Recheck comparable cost with the
upcoming landmark changes; do not attribute shared software timing variation to this AI edit.

User now identifies UFO/alien AND castle ruins as visibly unfinished. This is the next priority,
ahead of the remaining bunker maneuver. Baseline captures all passed existing image gates,
which plainly did NOT establish sufficient model quality; actual images inspected:
20261102T164635.754953Z-ufo,20261102T164705.302791Z-alien,
20261102T164734.974723Z-alien-examine,20261102T164804.967113Z-alien-hull,
20261102T164955.137584Z-castle under build/software-captures.
Observed: alien shoulder gaps/block-prism torso, flat faceted head/limbs; UFO random panel
checker coloration, square opaque burn tiles and segmented tube berms; castle checker-coloured
uniform wall columns/regular jagged cuts need close gate/courtyard review. Keep identities,
placements/collision/features; improve existing shapes/material/ground transitions. No new
landmark implementation yet. Sound closed by user's confirmation; no further audio work.

### UFO/alien/castle implementation continuation — ACTIVE

Current UFO code removes opaque whole-cell scorch, using continuous vertex-alpha ground fields
with an existing XNA NonPremultiplied/DepthRead pass. Ground never casts its own shadow. Actual
soil texture remains visible across feathered irregular edges. Continuous partly buried berms
now vary deposit radius and position; welded-to-surface seams and inset oval ports follow the
actual dented/tilted hull rather than floating unmodified profile radii. Panel/dome random
checker colors removed. Same placements, collision identity and content retained.

Release buildCPU0–3/-j3 and UfoScorchFieldTests passed. Two visual iterations inspected:
170448.966397-ufo/170505.703572-alien-examine then final
20261102T171114.833985Z-ufo,20261102T171130.806846Z-alien-hull,
20261102T171146.269258Z-alien-examine under build/software-captures.
Compared with the previous baseline: build/campaign-final-comparisons/ufo-ground-panels.png/.json.
4104→3646triangles,1→2main draws, no scorch shadow casting. Serial comparable LP8 benchmark
before/after120warmup/600samples running under build/campaign-performance/ufo-polish-{before,after}.
Entry binary preserved build/campaign-landmarks-entry-binary,sha765febe3241b39253cb4e27d1b8e97375a17a97350d07d1ad8a8b3648dbd8d33.
No build/test during the serial benchmark. Beforemean221.1909ms,p95252.3531ms; after pending.

Alien geometry extraction/new BodyBuilder and CPU tests are now written and wired in CMake but
NOT built or rendered yet. Existing rig/AI preserved; smooth shared-vertex normals from existing
SurfaceNormals, connected chest/shoulder/pelvis, rounded soles and separate palm/finger roots.
New castle-gate/castle-courtyard QA views written (preserve old wide camera); no castle mesh
correction yet. Next: finish benchmark, build/test new alien, capture its poses and castle close
baselines, correct actual defects, then qualify/commit/push coherent changes. Do not stop here.

UFO serial software benchmark completed safely: mean221.1909→210.2921ms(−4.93%),
p95252.3531→246.9948ms(−2.12%), RSS885812→884048KiB(−.20%). Same terrain36cells/11draws,
97palms/128canopy/552grass clusters; overall tracked submission range159–164in both, UFO+1draw.
Single serial pair is evidence against a material regression here, not a sustained speedup or
hardware claim. Both app/runtime identities unchanged during their runs; llvmpipe/Pixman and
final kernel records verified. Full report build/campaign-ufo-polish-performance.json.

Alien prototype now builds and AlienBodyMeshTests passed140poses/shoulder silhouette continuity,
finite/deterministic geometry and existing16000vertex budget. Actual images inspected:
20261102T172203.751006Z-alien,20261102T172220.255920Z-alien-examine,
20261102T172234.648033Z-alien-hull. Skin/head/limbs smoother, body attachments closed and palms/
soles rounded; no final animation/performance qualification claimed yet.

New old-castle close baselines20261102T172246.671414Z-castle-gate and
20261102T172259.453979Z-castle-courtyard reveal severe existing fins: old column code raises a
low endpoint to each course's bottom, emitting triangles ABOVE the broken top; upper lintels
also lacked bottom caps. Implemented bounded course clipping and valid cap spans in the existing
column mesher; no layout/heightfield replacement. CastleWallSectionTests passed. Matching actual
render inspection currently running. Next fix remaining material/gate defects only after review.

Castle course clipping actual render reviewed successfully: matching gate/courtyard sets
20261102T173001.918154Z-castle-gate,20261102T173013.469430Z-castle-courtyard and wide
20261102T173023.997120Z-castle under build/software-captures. Protruding fins disappear;
remaining real defects are checker-coloured flat masonry, pointed/ramped battlements and a
poorly supported rectangular half-lintel. Fix these next, keeping the existing castle plan.
Planned bounded material extension: optional CastleStone surface in existing StructureShadowReceiver,
not a new renderer; antialiased world-space courses/joints/grain with distance fade, keep wood
unaffected, preserve existing relight/shadows/wetness/fog. Derivatives must execute outside
nonuniform branches. Geometry: near-flat chipped stone tops per column and connected curved
left half-arch with walking clearance. No implementation of this next material/arch step yet.

UFO de10d0de committed; push session78202 must be collected. Alien/new castle QA and clip fix
remain uncommitted pending fuller qualification; no other work overwritten. User asked remaining
time: told approximate4–6hours including these models, actual bunker maneuver and final QA,
with no fixed completion promise. Continue work; do not end at this checkpoint.

Castle stone/arch material iteration reviewed:20261102T174227.300610Z-castle-gate,
20261102T174244.136333Z-castle-courtyard,20261102T174300.743034Z-castle. Same cameras.
Staggered antialiased limestone courses through optional existing StructureShadowReceiver
surface; no new renderer/texture/dependency/draw. Existing relight/shadow/wetness/fog retained,
derivatives execute unconditionally. Timber alpha254 marker bypasses coursing and final alpha
is restored1; loose individual rubble now also bypasses brick grid. Default Plain unchanged.
Near-flat chipped caps replace pointed merlons; left partial arch is joined to its spring and
has at least2.7m local clearance. Further final current-build captures pending after rubble fix.
Night and wet UFO controls actually inspected at20261102T174316.674160Z-ufo-t22.00 and
20261102T174334.389473Z-ufo-thunderstorm-wet: no scorch halos/opaque border/receiver failure.

Actual keep doorway collision defect reproduced BEFORE fix (ignored CPU probe:
build/campaign-castle-door-before.log,41/81centre-path samples blocked). Shared door dimensions
now govern drawn opening and collision proxy gap/jambs. CastleTraversalTests passed through
three orientations: outer gate, breach, keep doorway clear; actual PlayerMotion enters keep
while adjacent solid walls block. CastleWallSectionTests still pass.

Alien shoulder spheres were too prominent in actual prototype images. Removing them entirely
failed the silhouette invariant; corrected to shallow deltoids running down the arm below chest
top. Preserved test, no invariant weakening. Real walking QA uses existing steering/IK with one
clear fixed local route; no gameplay AI rewrite. New alien-walk64fixed updates/16stages camera
follows translation; feet/visible mesh/travel protocol is checked. AlienBodyMeshTests now also
exercise actual movement/contact (not merely Place(Walk) with speed0) and pass0.20s. Current
six-view final render loop session77331 was completed earlier; NEW active loop session in latest
tool output covers alien/alien-examine/alien-hull/alien-walk/castle-gate/castle-courtyard.
UFO push verified: develop and origin/develop bothde10d0de. Remaining code uncommitted until
visual/motion/performance/full-test qualification. Build CPU0–3/-j3; no native GPU testing.

### Current landmark qualification completed; campaign remains ACTIVE

Full Release build passed (CPU0–3/-j3), build/campaign-landmark-final-build.log.
Full approved non-display CTest88/88passed110.62s, build/campaign-landmark-final-ctest.log.
36display tests and two orphan image/startup validators still intentionally excluded from bare
CTest; graphical checks use the approved contained software supervisor, never physical AMD.

Final actual sets (all under build/software-captures, current model code):
20261102T175858.087902Z-alien,20261102T175910.693271Z-alien-examine,
20261102T175923.358737Z-alien-hull,20261102T175936.404110Z-alien-walk,
20261102T180008.290718Z-castle-gate,20261102T180024.914776Z-castle-courtyard.
Matched OLD alien renderer, with otherwise current code/identical walking fixture:
build/campaign-alien-20261102-baseline/software-captures/20261102T180610.556738Z-alien-walk.
All16stage positions/headings/speeds/feet/contact values match exactly; geometry1224→1918triangles
(3672→5754vertices), same one draw and unchanged16000vertex buffer capacity. Actual stride/start/
stop sequence inspected; new mesh smoother and connected; no claim of new world-foot IK.

Weather/night sets actually inspected:20261102T180647.318789Z-castle-courtyard-thunderstorm-wet,
20261102T180705.111891Z-castle-courtyard-t22.00,20261102T180719.946486Z-alien-t22.00.
Unlit ruin is deliberately dim; masonry/entrance silhouettes remain readable. No shader fallback,
new geometry fins, opaque scorch edges or emission artifacts observed. Current castle15902triangles
vs15882original, same one main draw. Loose stone blocks keep their original plain material,
not a smaller brick pattern painted across each individual block.

Reviewed contact sheets/indexes: build/campaign-final-comparisons/alien-anatomy.png/.json,
castle-repair.png/.json,alien-walk-comparison.png/.gif/.json. Static alien pairs also show the
intentional UFO background change; motion pair isolates the alien renderer. Isolated original
renderer binaries were built WITHOUT source rollback or dependency edits; exact old source,
compile/link commands and hashes: build/campaign-component-baselines-20261102/baselines.json.

Serial LP8 software component pairs,1280x720,VSync off,120warmup/600samples, no compiler/test/
other graphics workload: build/campaign-alien-castle-performance.json and
build/campaign-performance/{alien-body-before,alien-body-after,castle-ruin-before,castle-ruin-after}.
Alien mean221.8388→213.9396ms(−3.56%),p95256.7153→244.3755(−4.81%); tracked submissions
160–165both; peak RSS894248→927804KiB(+3.75%). Extra organic joint/palm/sole geometry fits the
existing buffer; no sustained/native speed or memory-allocation-cause claim. Castle close gate
mean236.8511→230.2508(−2.79%),p95281.7933→261.0985(−7.34%),RSS885580→880304KiB(−.60%),
submissions144–147both. Single serial pairs show no material frame regression here; host timing/
allocator variation remains. App+runtime identities unchanged; renderer/software/Pixman and final
kernel records checked in every run. No new10%+frame/memory regression requiring simplification.

Next unfinished original priority: actual driven bunker maneuver and forward escape, then final
holistic/criterion review. Computer room has desks/chairs across4.18..8.25m, so do NOT qualify a
route that drives through that scenery just because room-floor collision lacks furniture shapes.
Service workshop has a clear centre (cabinets at6.46+, bench across3.15..5.95/depth34.25+).
Investigate a continuous driven turn there, retaining existing physics and real footprint checks;
previous exit test starts reoriented and does not prove a turn. Sound closed by user confirmation.

Alien production mesh/CPU qualification committed as1ea80289. Castle geometry/material/door
collision and both new landmark capture fixtures are the following cohesive commit; all were
covered in the88-test run and actual screenshots above. Main switch indentation cleanup is
whitespace only. Continue with bunker route; do not close campaign after these commits.

### Bunker continuation — actual missing wall collision reproduced (ACTIVE)

CPU probe campaign-bunker-roundtrip-before.log confirms capsule passes closed drawn dividers
at local(1.36,12) and(1.36,28), whereas the real openings are depth14..19/30..33 right and27..30
left. New shared door dimensions govern renderer openings/frames and capsule/whole-vehicle
collision. Floor support remains continuous beneath walls; above-roof height guards preserved.
SAT detects a thin divider enclosed between the old point samples. New BunkerDoorCollisionTests
and existing bunker layout/support tests pass. Further final geometry-frame sharing qualification
pending. Baseline actual service room: software-captures/20261102T185902.547842Z-bunker-service.

Reverse-only full trip failed on the existing uphill rear-drive traction limit and eventually
overturned (campaign-bunker-roundtrip-before.log); no physics constants were changed to conceal
this. A real continuous approach/workshop multipoint turn/forward escape is now covered by
BunkerTurnInspection (inputs only, shared CPU/rendered fixture). CPU tests at.05/.1seconds exit
depth-4.10/-4.24, with142/74reverse frames, maximum absolute tyre-plane gap.127m, zero colliding
physics substeps, no overturn/roof contact. Full conservative footprint also clears the rendered
workshop cabinets, bench and distribution board. No re-placement or Right() after initial start.
Rendered800-frame/16 stage qualification is running through the approved software supervisor;
NOT yet claimed passed or reviewed. CPU fixture does not prove final rendered scene by itself.

First continuous rendered run20261102T190533.006952Z-quad-bunker-turn is FAILED, not accepted:
the240-second supervisor limit expired after13 captured stages (kernel-after.json empty). Actual
review of its PNGs found the chase camera interpolating through a divider despite valid floor
endpoints. New FitBunkerChaseCamera sweeps the full camera sphere sightline after smoothing;
production BunkerLayout remains authoritative. Deterministic endpoint/occlusion and continuous
CPU camera regressions pass. New final rendered run will check accumulated camera collision/
occlusion counts as well as vehicle contact. Its fixed800 frames have a600second software-only
ceiling; ordinary capture timeouts remain65seconds. No hardware fallback or weakened image gate.
Bunker frame dimensions now also share the exact rendered jamb extents, rather than approximate
wall strips. Earlier failed compilation due an incomplete QA helper type was corrected by an
explicit header include; final sequential rebuild still required after the last header edits.

Intermediate camera run20261102T192113.611608Z-quad-bunker-turn passed the strict800-frame
protocol: all16 stage images, zero accumulated vehicle/camera wall violations, completed real
entry/turn/forward exit, app 81127cd810a2f03a5a9f807968189036be61c1e113ad7ea22ac6c09dd37ad603
unchanged on verified llvmpipe/Pixman. Kernel delta contains one informational PCI PME
spurious-interrupt record and zero GPU faults; it is not empty. It is NOT final visual acceptance:
actual review found decorative blank panels crossing the open computer/workshop doorways and
the camera inside a hanging lamp at exit stage12. Source now suppresses unsupported panels/
fasteners through shared door spans and retains65 cm clearance below the ceiling slab for the
camera and near plane, with an explicit lamp-height regression. All wall/vehicle movement unchanged.
Old failed timeout set remains190533; intermediate 192113 remains evidence rather than relabeled
final. Exact same-byte intermediate binary saved as campaign-bunker-panel-before-binary.

Before the panel/lamp follow-up: full Release build passed;90/90 safe CTests passed in 128.25 s,73/73
harness and11/11validator tests passed, fingerprint238be1fec0a0cdd7 unchanged. CPU-only camera
fit cost over288 cases (65 occluded with old endpoint-only method,0 after) is11.82/12.53/12.16 µs
per fit across three runs. No frame/GPU speed comparison is claimed; scene geometry/buffers
are unchanged by the camera correction. Final follow-up rebuild/render review/tests pending.

### Bunker visual/functional correction qualified — campaign still ACTIVE

Final app b3819374a0c3c5953c3d8ce352a6fdfb231779b9f76172a92c7d3a83b03bae51,
Release/OpenGL33, CPU0–3/-j2 final rebuild: campaign-bunker-panel-exact-build.log. One earlier
full build ended 143 without a compiler diagnostic; no compiler remained, and its sequential
retry plus final exact rebuild succeeded. No external dependency source modification.

Final software set 193404.936591Z-quad-bunker-turn (under build/software-captures) PASSED:
800 rendered frames/800 fixed 0.1 s updates,16 stage PNGs, full entry/multipoint workshop turn/forward
escape to depth −5.41552 m; all16 vehicle stage values EXACTLY match the intermediate 192113 set.
Zero accumulated vehicle-wall, camera-wall/sightline and overturn violations; worst absolute
tyre-plane floor gap 0.125568 m. Every CPU physics substep also fits actual doorways and clears
the workshop cabinets/bench/board, at both 0.05/0.1 s. No re-placement/righting/engine-force change.

All16 final sample images actually reviewed in bunker-turn-final-{1,2,3,4}.png under
build/campaign-final-comparisons. Camera stays with the rider through the turn; open computer/
service doors no longer contain floating panels, fasteners or a low cable tray; uphill camera
remains below hanging fixtures. Close overhead lights remain visible in a low passage, without
the stage12full-screen lamp interior. Comparison bunker-turn-repair.png/.json explicitly uses
separate original/intermediate baselines for each issue, not falsely one shared before binary.
Intermediate failed/rejected histories retain separate visual-review.json decisions. Final
visual-review.json is accepted; renderer is runtime-verified llvmpipe/Pixman, artifact unchanged
and kernel-after.json empty. Software only, no affected physical GPU workload.

Bunker mesh 7664→7604 triangles, same two draws and existing buffer guards/materials. Shared
Doorways/CorridorOpening and exact jamb extents govern renderer and collision; horizontal SAT
prevents thin-wall point-sampling misses. Floor support/roof guards remain unchanged. New
BunkerDoorCollisionTests and BunkerTurnTraversalTests pass along with existing bunker tests.
Final full 90/90 safe CTest rerun passed in 128.38 s (campaign-bunker-qualified-safe-ctest.log);
world report again valid with unchanged
fingerprint 238be1fec0a0cdd7. Original criteria/final holistic review are still pending: do not
complete the goal or stop at this bunker milestone. Next: final same-build environment/landmark
view matrix, visual inspection, acceptance ledger, source/state cleanup and commits/push.

Latest clearance-bound CPU measurement (after final source build),
build/campaign-bunker-camera-qualified-cost.json: 288 cases, 65 occluded with old endpoint-only
placement, 0 after full fit; three incremental fit measurements 9.11/7.26/7.21 µs. No compiler,
test or graphics workload overlapped that measurement. This is CPU helper cost, not a native
frame-time claim or a substitute for the earlier campaign software before/after pairs.

### Final same-build holistic review completed; live closure still pending

Source commit 83019563 is signed and pushed; origin/develop matches it. Final app remains
b3819374a0c3c5953c3d8ce352a6fdfb231779b9f76172a92c7d3a83b03bae51. The serial matrix
build/campaign-final-20261102-views.json contains 27 successful same-build software scopes and
47 saved PNGs:27 main images,8 resident stages,8 bear stages and4 stream stages. ALL were actually
reviewed in campaign-final-20261102-{1..5}.png plus resident/bear/stream-motion sheets under
build/campaign-final-comparisons. Rain shelter, night castle, whole-island, forest floor/edge,
stream, coast, underwater reef, cave roof/interior, hut, lighthouse, repaired alien/UFO/castle
and bunker/cave/mine quad controls remain coherent. No new serious artifact was identified.

Machine audit: build/campaign-final-polish-evidence-audit.json; all renderer logs verify actual
llvmpipe, private Weston Pixman, bwrap private devices/PID lifetime, unchanged app bytes, fixed
clock and no new GPU fault. 19 main images are pixel-identical to the previous same named view.
The other 8 retain matching clocks but include intentional intervening landmark/wildlife/material
changes; both changed-controls sheets were actually reviewed, not blindly declared improved by
pixel difference. This final matrix complements the earlier isolated before/after comparisons.

Current same-app real keyboard cave scenario passed: build/software-cave/20261102T195302.462188Z.
All 6 PNGs actually reviewed in campaign-final-20261102-live-cave.png. Ground/ceiling/landing/exit
remain contained; run/input/cave protocols pass and kernel-final.json is empty.

Extra final stream repetition exposed a launcher wall-time limit, not a new movement defect:
195542.643933Z was interrupted by parent SIGTERM before a final result; its raw game trace
already reaches signed-across 2.48474 m. Preserve as NOT accepted. No owned game remained;
post-termination journal is empty and app identity unchanged.195925.053662Z then failed the
35-second wait while still advancing at signed-across 1.37928 m, with support error ≤4.77e−7 m and
empty kernel delta. Existing IslandStreamCourseSurvey already covers both directions at 60 Hz
and 100 ms; no duplicate C++ test was added. Independent ignored CPU probe confirms the exact
−110m/centre−4m start crosses in 8.5167/8.55/8.6 simulated seconds at 1/60, 0.05 and 0.1 s, with identical
340 terrain rejections. Variable software throughput had supplied only about 6 simulated seconds.

Only tools/run_fullscreen_software.py changed: default waits remain 35 s; the one held stream
walk now allows 90 s inside the unchanged 240 s supervisor. Same far-bank/channel/support/mode/
monotonicity criteria remain.73/73 CPU harness tests pass after it. New actual stream scope
201116.059915Z PASSED unchanged app/clean kernel; raw proof and before/after images are in
build/software-stream-crossing/20261102T201116.059915Z.
No application physics or sound change. Sound remains closed by the user's confirmation.

Final flight scope ACTIVE: build/software-flight/20261102T201325.295494Z, tool session 87577,
log campaign-final-live-flight.log. Next: collect/inspect it and the stream images, finalize the
original sections 1–24 criterion evidence and remaining limits, commit/push launcher+qualification docs,
verify clean/synced state, THEN assess goal completion. Do not stop at this checkpoint.

## Final original-superprompt acceptance — 2026-11-02

This is the current full-campaign assessment. It supersedes the earlier premature completion
conclusions and subsequent ACTIVE checkpoints. The user-identified alien/UFO/castle defects,
real bunker door/camera defects and final software stream deadline have now been corrected and
qualified. Completion is based on the whole original scope and the evidence below, including
actual rendered inspection; it is not inferred from a build, one milestone or updated goldens.

### Exact final artifacts and available validation

- Production source: `8301956313e88863593f37db1e739e3dc90cf5f4`, already pushed to develop.
  The following launcher/docs commit changes only a scoped software-test wait and these records.
- Release/OpenGL33/GCC14/Ninja app:
  `b3819374a0c3c5953c3d8ce352a6fdfb231779b9f76172a92c7d3a83b03bae51`.
  App and CNA runtime bytes remained unchanged through all final captures/live scopes.
  Identity: `build/campaign-bunker-final-identity.json`; no dependency source was patched.
- Full approved CTest: **90/90 passed, 128.38 s**, `build/campaign-bunker-qualified-safe-ctest.log`.
  After the launcher-only stream wait correction, its **73/73 CPU tests** and the combined
  `IslandStreamCourseSurvey`/`IslandSoftwareCapturePreflightTests` CTest scope pass again.
  Image-validator tests: **11/11 passed**. Exact commands are recorded earlier in this file.
  Final scoped log: `build/campaign-final-stream-scoped-ctest.log`.
- **36 display CTests and two orphan image/startup validators were deliberately excluded** from
  bare CTest. They are not reported as passed; the supervised software paths produced their
  own artifacts. No direct AMD workload was launched. All compilation used CPU0–3 and -j2/-j3.
- Final world report: `build/campaign-bunker-qualified-world.json`, valid fingerprint
  **238be1fec0a0cdd7**, unchanged. Current game/tests/tools inventory: 780 C++ files, 158427 lines;
  IslandGame.cpp 758372 bytes and CoreTests.cpp 913532 bytes. CoreTests.cpp was not enlarged.

### Final actual visual and functional evidence

`build/campaign-final-polish-evidence-audit.json` indexes the current app, renderer strings,
private-device/PID containment, Pixman compositor, clocks, kernel results and all raw paths.
Every app renderer is verified **llvmpipe LLVM 19.1.7/Mesa 25.0.7, OpenGL33**. PCI adapter metadata
was not mistaken for the renderer. No final scope reports a new GPU fault. The three final
live scopes and the final bunker turn have empty post-cleanup kernel deltas. An intermediate
camera run had one informational PCI PME interrupt; it was not called an empty delta or a
GPUVM fix. The physical AMDGPU incident remains OPEN.

- **27 deterministic scene scopes / 47 PNGs**, all actually reviewed: five
  `build/campaign-final-comparisons/campaign-final-20261102-{1..5}.png` sheets plus all eight
  resident stages, eight bear stages and four stream stages. Includes the island overview,
  forest floor/edge, stream valley, ridge/coast/beach, shallows, underwater reef, wreck, cave
  roof/interior, hut, resident/bear, castle wide/gate, UFO/alien, lighthouse, bunker workshop,
  three quad entries, rain shelter and night castle. 19 main controls match their previous same
  named view pixel-for-pixel. The eight changed controls retain matching clocks and were
  separately compared and inspected; pixel difference was not treated as proof of improvement.
- **Continuous bunker journey: 800 rendered frames / 800 fixed updates / 16 saved stages**,
  `build/software-captures/20261102T193404.936591Z-quad-bunker-turn`. Real approach, descent,
  doorway, multipoint turn and forward exit; no re-placement/righting after start. Exit depth
  −5.41552 m; worst absolute floor gap 0.125568 m; zero accumulated wall/overturn/camera-sightline
  violations. Vehicle stage values exactly match the intermediate control. All 16 stages
  reviewed; repair sheet `build/campaign-final-comparisons/bunker-turn-repair.png` explicitly
  identifies the distinct original/intermediate baselines for its three corrections.
- **Real keyboard cave entry/back/ceiling/landing/exit:**
  `build/software-cave/20261102T195302.462188Z`, passed; all six images inspected.
- **Real keyboard stream crossing:**
  `build/software-stream-crossing/20261102T201116.059915Z`, passed; 35 samples from signed-across
  −3.09522 m to +3.16295 m, maximum support error 4.76837e−7 m. All three images inspected.
  The previous interrupted and 35-second timeout runs remain explicitly failed/unaccepted.
  CPU checks prove the route needs 8.52–8.60 simulated seconds across three timesteps; only
  this software wait is now 90 s, within the unchanged 240 s supervisor. Physics, support and
  far-bank/channel/monotonicity criteria were not changed to make the test pass.
- **Real keyboard flight/held ascent/turn/100m limit/landing/fullscreen controls:**
  `build/software-flight/20261102T201325.295494Z`, passed; 69 flight samples, maximum above-ground
  height 99.999 m; all seven images inspected. Landed image and control transitions agree.
- The final capture matrix, bunker journey and three live scopes retain 80 PNG files in total
  (including the normal final-frame duplicate of the last bunker stage). Earlier deterministic
  before/after images and rejected experiments remain preserved. No golden was blindly replaced.

### Original criteria reviewed across the complete campaign

Comparison names below are under `build/campaign-final-comparisons/`; their JSON indexes retain
matching camera/configuration/raw paths. Earlier detailed stage records give the implementation,
RED→GREEN evidence, geometry/resource costs and failed attempts. The fresh final views above
check that the accepted changes still work together on the final production app.

| Original sections/criteria | Implemented or retained after investigation | Evidence and bounded acceptance |
| --- | --- | --- |
|1–4 context, preservation, investigation and safety|Reinspected actual sources, docs, incident, galleries, tests and render paths; preserved the existing C++/CNA architecture, user work, heightfield, features and identity. External dependencies/system configuration read-only; no merge/reset/clean or physical GPU stress.|Entry work preserved in ad48d3d4; actual counts/branch, incident preflight, runtime/containment records and final world fingerprint above.|
|5.1–5.2 forest organization/layers|Retained existing correlated moisture/drainage/exposure/species placement; corrected aerial canopy/palm reach and LOD with existing buffers, keeping clearings and species population. Layers remain selective rather than blanket clutter.|forest-review.png,aerial.png,palm-aerial.png,canopy-lod.png; fresh overview/jungle/clearing/floor/stream and live aerial inspection.|
|5.3–5.6 tree form, forest floor, transitions and grounding|Mangrove/stilt/root contacts follow real terrain/banks in visible/shadow passes; moss/grass pigment uses shared canopy/moisture suitability; wet organic litter retains contrast. Existing crown/branch identities and path exclusions retained.|trunk-roots.png,trunk-root-environment.png,contact.png,wet-litter-ground.png; grounding/visibility tests and deterministic placement controls.|
|6.1–6.3 terrain/geology/soils|Kept authoritative heightfield and geological context; corrected rock projection cancellation and mine slope/earth treatment rather than adding indiscriminate terrain noise or replacing the pipeline.|geology-projection.png,geology-projection-detail.png,mine-grounding.png; close/coast/overview matched comparisons and current ridge views.|
|6.4–6.6 props/caves/view distances|Corrected rock/root seating and bunker approach; retained covered cave roof after actual overhead/interior review; corrected supported-feet cave collision and downhill contact.|rock-contact.png,trunk-roots.png,bunker-apron-comparison.png; current cave-top/interior and live cave/flight, relevant CPU support/collision regressions.|
|7.1–7.4 ocean/shallows/surf/exposure|Retained existing multiscale directional/spatial wave model after inspection; reduced rigid seabed caustic grid and corrected foam pigment relative to opacity. Depth absorption, sheltered/exposed fields and existing surf architecture retained.|water-review.png,water.png,reef-habitat.png; final water/coast/wreck/reef views and stream motion samples. No new fluid simulation or broad shader brightening.|
|7.5–7.7 underwater habitats/tides/exploration|Retained habitat masks/fish depth and seated reef props; tide-relative diving/telemetry fixed; actual held high/low states qualified through existing TideCycle with first/last-frame proof.|tide-extremes.png,fish-swim-final.png,earlier six high/low pairs and actual dive/resurface run 20261102T133945.409145Z; final underwater view retained. Not a continuous twelve-minute tide recording.|
|8.1–8.3 wildlife anatomy/gait/weight|Bear head/fur and continuous anatomy corrected; boar/deer gait follows accepted travel/support; bedded deer rises before alert turns and uses short steps. Bird feather planes now follow supporting wing spans; fish/flight motion reviewed.|bear-face-flow.png,boar-gait-walk.png,deer-gait-walking.png,deer-alarm-recovery.png,bird-feather-final.png,fish-swim-final.png; existing animal tests, final bear stages, source/motion controls.|
|8.4–8.6 idles/habitat/interaction/density|Stopped animals do not falsely travel-step; turtle nesting validates actual habitat/capsules; existing animal preferences/populations retained. No new species or zoo-like population increase.|Turtle seed/control sheets, deer/boar stop stages, representative goat/crocodile/monkey/bird/fish reviews; deterministic world/population tests. Procedural turn/acceleration slip remains bounded below.|
|9.1–9.5 resident proportions/forms/materials/animation/scope|Rounded connected shoulder/neck/forearm/face forms, sleeve normals, waist/cloth transition and gait presentation; existing rig, route, woven material and character identity preserved.|resident.png,resident-waist.png,resident-motion-final-review.png; eight fresh final stages and earlier dawn/night/work poses. Not a new avatar engine.|
|10.1–10.4 art direction/light/shadows/materials|Unified restrained soil/leaf/rock/cloth/fur/water response; corrected aerial SunShadowMap focus and shared contact transforms; optional castle stone material uses existing StructureShadowReceiver, preserving other material defaults.|lighting-review.png,shadow-focus.png,shadow-focus-live.png,geology and resident comparisons; clear/night/wet/interior matrix and 19 exact final controls.|
|10.5–10.6 LOD/weather|Existing LOD architecture kept, with cheaper aerial index ranges and one instance upload; canopy/rain shelter/eaves/drips/wet floor and cave puddle-rain clipping corrected.|canopy-lod.png,rain-eaves.png,rain-cohesion.png,cave-rain-rings-detail.png; actual final flight, wet hut and night castle. No disabled LOD or extra AO/shadow engine.|
|11.1–11.4 landmarks/composition/grounding/character|All meaningful landmarks retained. Mine earth/bunker apron grounded; UFO scorch edges/hull details fitted; alien anatomy connected/smoothed; castle fins, battlements, partial arch, masonry and blocked keep door repaired. Existing vegetation/negative space preserved.|ufo-ground-panels.png,alien-anatomy.png,alien-walk-comparison.png,castle-repair.png,mine-grounding.png,bunker-turn-repair.png; fresh wide/close landmark views. No island enlargement or attraction inventory expansion.|
|12.1–12.3 walking/structures/swimming|Bounded player integration and supported-feet sweeps fix observed stream/cave issues; keep and bunker door collisions agree with openings. Tide-relative swimming retained after actual dive checks.|StreamCourseSurvey bidirectional 60 Hz/100 ms, CaveLayout/PlayerMotion and CastleTraversal tests; final real cave/stream and earlier dive/resurface proof.|
|12.4–12.5 vehicles/regression|Bunker wheels anticipate the floor before body entry; apron matches packed terrain. Shared doors/jambs and whole-footprint SAT block actual dividers; post-smoothing camera stays under fixtures and clear of walls. A real driven turn/forward escape is proven.|BunkerQuadSupport/BunkerDoorCollision/BunkerTurnTraversal tests, final 800-frame journey; quad cave/mine controls and existing footprint audits. Rear-drive reverse-only uphill limit retained, not hidden by a force hack.|
|13.1–13.4 performance/resources/backends|Measured controlled software pairs and investigated material regressions/outliers; geometry reuse/budgets retained. No brute-force population/shadow/texture increase or renderer replacement.|Performance table below and earlier stage reports. Actual current backend only; CPU helper cost separately labelled, not a native FPS claim.|
|14.1–14.4 code health/determinism/docs|Focused helpers/mesh extraction/tests outside giant files; no mass refactor, hidden global ecology or dependency additions. Fixed-seed world unchanged; concise current result plus historical evidence retained here.|AlienBodyMesh,CastleWallSection,BunkerChaseCamera/BunkerTurnInspection and focused tests; fingerprint/counts above; current source review/diff check.|
|15.1–15.5 visual QA|Repeated baseline→focused implementation→build→software render→actual inspection→correction→regression loops; rejected intermediate states retained, including final camera/lamp/panel discoveries.|All named comparison indexes, current 27-scope/47-image audit, 16 bunker stages and 16 live screenshots. Exact matching complements visual inspection rather than replacing it.|
|16–18 phased autonomy/quality|Proceeded through vegetation, geology, water, lighting, wildlife/resident, landmarks and traversal instead of stopping after each stage. Preserved all required safety/architecture/function/performance/determinism constraints.|Chronological stage records, user-directed landmark repairs, the additional real bunker correction and this full acceptance assessment.|
|19 known issues|Application corrections were kept separate from framework/driver/platform limitations; no CNA defect silently patched or incident marked resolved.|BUGS.md and incident documents remain authoritative; precise remaining limits below.|
|20 holistic integration|Reviewed the complete final island from ground, coast, interiors, underwater and air; clear/rain/night scenes, wildlife/person, structures and real traversal considered together.|Current five holistic sheets, all motion sheets and live cave/stream/flight sheets; no new serious artifact identified.|
|21 stopping criteria|Major observed in-scope defects addressed, corresponding comparisons improve, relevant tests pass, no serious new regression remains identified. Further full rig/terrain/renderer redesign would add disproportionate scope/risk rather than close an observed required defect.|This whole-campaign ledger and current actual evidence, not an individual milestone. No zero-defect or photorealism claim.|
|22–24 deliverables/principles|Actual maintainable repository changes, paths, tests, renderer, measured costs, honest limits and commits provided; existing application identity/content preserved.|Source commits below, final user report, clean/synced Git verification after the concluding commit.|

### Performance interpretation and remaining limits

Newest isolated pairs: 1280×720, LP8, VSync off, 120 warmup/600 samples, serial runs without another
campaign build/test/render workload. Single pairs do not establish universal native speedups.
Software-process RSS is not GPU VRAM.

| Isolated component | Mean ms before→after | p95 ms before→after | Complexity / memory |
| --- | --- | --- | --- |
|UFO|221.1909→210.2921|252.3531→246.9948|4104→3646 triangles; main draws 1→2 for feathered ground alpha; RSS 885812→884048 KiB.|
|Alien body|221.8388→213.9396|256.7153→244.3755|1224→1918 triangles, same draw/buffer capacity; RSS 894248→927804 KiB (+3.75%), allocation cause not established.|
|Castle|236.8511→230.2508|281.7933→261.0985|15882→15902 triangles, same main draw; RSS 885580→880304 KiB.|
|Bunker follow-up|No new comparable full-frame speed claim|CPU camera fit 7.21–9.11 µs/call|7664→7604 triangles, same two draws; 288 camera cases, 65 old occluded sightlines→0 after fit.|

Raw latest metrics/provenance: `build/campaign-ufo-polish-performance.json`,
`build/campaign-alien-castle-performance.json`, `build/campaign-bunker-camera-qualified-cost.json`.
Earlier controlled forest/aerial/soil/rock/rain/animal pairs and outlier investigations remain
in their stage records. No new material frame regression appears in the measured component pairs;
this does not qualify hardware timing, sustained memory behavior or every possible viewpoint.

Meaningful limits are explicit:

- Application/simulation: land-animal turning/acceleration can briefly slip; no persistent
  world-foot IK is claimed. Reverse-only rear-drive climbing of the bunker ramp remains limited;
  real turning and forward escape now work. Stylized forms are intentionally retained.
- External CNA/Android: existing surface-lifecycle race and stock-skinned guest shader/uniform
  issues remain as recorded in BUGS.md. This campaign did not patch their dependencies or claim
  fresh Android qualification.
- Driver/hardware: AMDGPU GPUVM/flight incidents remain OPEN. No physical GPU qualification,
  native FPS/VRAM result, kernel/firmware/driver change or attempted fault reproduction.
- Unverified platforms/coverage: no current Debug/Web/GLES/Android/other-backend/stock-fallback
  visual matrix. Representative motion/high-low states do not exhaust every species/tide/time.
- Automation history: one parent SIGTERM remains unexplained; interrupted/failed runs were
  preserved and excluded. Final supervised scopes all completed; the measured slow stream
  wall-time case is corrected without weakening its functional assertions.

### Repository delivery and conclusion of the recorded campaign

Branch develop. User entry work and all source changes are preserved in signed, pushed commits
ad48d3d4, 5f5f14b4, 01cb3778, de10d0de, 1ea80289, 23c99801 and 83019563. Sound was closed by the user's
confirmation and was not reopened during final movement qualification. The final following
commit contains only the scoped launcher deadline and final qualification records; no merge or
force push. Its exact hash and clean/origin-synced state are verified immediately after commit
and reported to the user, avoiding a circular self-hash in this file.

The original criteria have now been assessed with actual evidence across the full campaign.
Available safe qualification is complete; remaining platform/driver/simulation limits above are
not represented as passed or fixed. No additional required in-scope implementation remains
identified by the final review. Goal completion is assessed only after this ledger is written
and the concluding repository delivery is verified.

## User-requested continuation from f7446521 — ACTIVE

Entry develop/f7446521, clean worktree, previous source/qualification pushed. Original attachment
and relevant README/PLAN/BUGS/critical GPU incident reread; dependencies remain read-only. New
active goal created at the user's explicit request. Do not close it at the crocodile milestone.

Initial actual crocodile static baseline: build/software-captures/20261102T205620.561431Z-crocodiles,
verified software capture passed and image inspected. Existing model/armor/head improvements
retained. Source evidence: gaitPhase advances by dt*(1.1+2.2*speed), even when speed is zero;
renderer/shadow stride uses(pair*2+side)*pi, putting same-side fore/hind legs in phase and no
foot lift. Static legacy QA shows the swimming subject and cannot qualify bask/crawl. Adding
one160-frame/16stage crocodile-land inspection through existing camera/clock/capture facilities
for a real natural bask→crawl transition, not a new wildlife/animation engine. Implementation
changes still pending baseline inspection. Next: baseline fixture build/capture, confirm defect
visually, then shared pose/contact/timing correction and focused regression coverage.

### Qualified crocodile land-contact correction — 2026-11-02

Observed before fix, not a hypothetical audit: all16 natural bask/crawl stages in
`build/software-captures/20261102T210527.520544Z-crocodile-land` were captured with the
new inspection clock but the old animal code. During zero translation the basking subject
rotated almost half a turn and pedalled. Existing static `crocodiles` frames show a swimmer,
so could not qualify this land defect. Actual pre-fix log is rejected by the new semantic gate;
`build/campaign-croc-idle-red-test.log` records the CPU regression failing before the repair.

Implemented shared small CrocodilePose helper for renderer and shadow: four-beat overlapping
stance, terrain-relative planted soles and raised return, phase calibrated to accepted movement
and turning. Resting non-alert bask no longer steers toward its own anchor. Independent tail
phase preserves swimming propulsion and gives only a subdued resting tail. Existing anatomy,
habitat/steering limits, population, geometry and draw budgets retained. Foot-height queries
use the existing rendered cache, with analytic fallback only when needed.

Release/OpenGL33 final app1155d0a5e8fc4abebbb07e8d5de28e177b8a62604395d2cddd481a0cd0a8fa37;
CPU0–3/-j2 build passes (`build/campaign-croc-final-build.log`). Full safe suite **91/91 passes
in108.56s** (`build/campaign-croc-final-safe-ctest.log`);3 focused tests,74 launcher tests pass.
Pose regression covers1879 planted/317 swing contacts on actual stream-patched drawn terrain,
straight stance compensation and the same shadow joints; fold-height swimming control retained.
No display-labelled/native-GPU test or the two orphan capture validators was run.

Final software scopes: `build/software-captures/20261102T212524.093381Z-crocodile-land`
(160 rendered/160 updated frames,16stages; natural crawl travel3.3034m), and
`build/software-captures/20261102T212602.052777Z-crocodiles` (legacy swimming control).
Both pass verified llvmpipe/Pixman/private-device containment, unchanged artifact and empty final
kernel gates. All16 land stages reviewed in `build/campaign-croc-land-pairs-{1..4}.png`; the
optimized final capture is **16/16 byte-identical** to those reviewed after images. Swimming
control separately inspected: tail/awash back/folded limbs preserved. First8 before/after frames
share camera and position; later views use the same translation-follow camera policy but animal
paths differ because the incorrect resting rotation was removed. No different route is presented
as an isolated mesh comparison. Land sequence ends in Swim behavior on the bank; actual water
posture is qualified only by the separate swimming control.

CPU3-pass isolated 10,000-update/full-tier3-animal measurement: median18.5377→17.9574us/update,
within run variability (`build/campaign-croc-cpu-final-comparison.json`). An initial unoptimized
version added4.17us/18.4%; investigation removed four unused procedural fallback calculations.
No native FPS or general speedup claim, no new triangles/draws, no renderer/dependency change.
Persistent world-space foot IK during turning/acceleration is still not claimed.

Next original priority already investigated: elephant placed soles differ from the rendered
terrain by −.183936..+.0451078m over5249 real route contacts (CPU probe). Baselines inspected:
`build/software-captures/20261102T212612.591614Z-elephant` and
`build/software-captures/20261102T212622.615210Z-elephant-close`; both safe scopes pass, final
kernel intervals empty. Existing static elephant QA actually remains standing during its short
capture, so add a bounded natural stand→walk inspection before correcting ground sampling.
Goal remains ACTIVE; continue this priority, then reassess the original whole-campaign ledger.

Elephant inspection progress: first natural stand→walk baseline
`build/software-captures/20261102T213339.879980Z-elephant-walk` passes and all8stages inspected.
It proves real transition but its south-east site has only3mm support discrepancy, so it is not
presented as visible proof of the18cm defect. Actual route audit found a standing contact error
−.203108..+.0607271m at the existing waypoint220.704,59.1376. Move only the new inspection
there (legacy cameras unchanged), capture its pre-fix result, then repair ground sampling.
One initial QA build failed the exhaustive enum switch warning in Main; missing name case was
added and retry passes. Crocodile stage delivered in8a92339c, confirmed pushed/synced.

### Qualified elephant grounding and natural walk inspection — 2026-11-02

Actual world-obstacle route selected the waypoint224.568,60.1729 near the requested220.7,59.14.
Its matched pre-fix64-frame capture is
`build/software-captures/20261102T213751.431284Z-elephant-walk`: planted-sole error
−.0884619..+.0446463m, visible buried/raised contact; all8before stages inspected in the pairs
below. The separate unobstructed CPU route probe measured−.183936..+.0451078m and its stationary
waypoint−.203108..+.0607271m. These different workloads are explicitly not conflated.

ElephantMotion now takes the existing RenderedTerrainSurface after local patches are installed.
Body support, all four soles, trunk clearance and bathing depth use the same Ground helper with
analytic fallback outside the mesh or when unset. Route/obstacle selection, anatomy, locomotion,
behavior, draw budget and renderer are retained. The first exact-contact regression failed
(`build/campaign-elephant-focused-first.log`): the old slow-step override labelled a swinging
foot planted while retaining its lift. Such feet now settle fully below5% walking pace.
Final dedicated test checks5498support/1710swing samples, zero planted error over90simulated
seconds, a local replacement patch, shadow endpoints, navigation preservation and null fallback.

New `--viewpoint elephant-walk` uses an existing route waypoint and64normal fixed-clock updates,
8stages, a low fixed reference camera and translation follow. It naturally waits the remainder
of the20s standing pause and starts walking without overriding AI. The old static wide view
finishes during the pause; its old camera is preserved and its source comment corrected.
Strict capture validation rejects the actual pre-fix log for disagreement with drawn ground,
and rejects missing stages, nonfinite fields, unsupported feet or omitted real walking.

Final app367810a6fe4454625b953641c0e2125d47c592edcee968ad2d67893c2e30efce,
CNA runtime950faa719ec2fb51cde7220d3e28100f2609731ede56c7cf4088bda0d2dcde5c.
CPU0–3/-j2 Release/OpenGL33 build passes (`build/campaign-elephant-final-build.log`);
**92/92 safe tests pass in151.45s** (`build/campaign-elephant-final-safe-ctest.log`, functional
run concurrent with software captures, not a comparable performance measurement). Launcher75/75
pass. No physical GPU test, driver change, external-source change or new graphical resource.

Final software scope `build/software-captures/20261102T214425.891885Z-elephant-walk`:
64rendered/64updated frames,8ordered stages,0.97501m travel, four resting contacts then3/2walking
contacts, all planted sole errors0. All matched8before/after stages reviewed in
`build/campaign-elephant-ground-pairs-{1,2}.png`. Their entire upper200px background is8/8exact;
reference camera/seed/clock/lighting match, XZ path retained. The visible change is local body/foot
support; no anatomy redesign or claim to have solved persistent stance slip on acceleration.

Additional inspected safe controls:
- `build/software-captures/20261102T214453.478972Z-elephant-close`
- `build/software-captures/20261102T214509.950953Z-elephant-trumpet`
- `build/software-captures/20261102T214527.262677Z-elephant-bathe`
- `build/software-captures/20261102T214544.182870Z-trex-close`
- `build/software-captures/20261102T214600.674991Z-trex-rest`
All6scopes pass llvmpipe/Pixman/private-device containment, unchanged identities and final kernel[].
TRex representation/rest retained after inspection; its separate CPU probe reports−.0461..+.0554m
ball-support deviation, not the elephant's larger contact problem. No speculative remodel done.

Isolated CPU3×20,000updates: elephant median12.8734→.870119us/update, using the existing cached
heightfield instead of repeatedly evaluating analytic terrain (`build/campaign-elephant-cpu-comparison.json`).
This is one simulation component, not a93%application/frame speedup. Draw/mesh budgets unchanged;
only a non-owning ground pointer is added to this one motion object. Full-frame software pair
and fresh whole-island review are the next qualification work; goal remains ACTIVE.

### Intermediate holistic qualification and final observed support follow-up

Commit76197fdc was pushed and worktree verified clean. App367810a6 completed24whole-island
software scopes/60PNGs; all24main images reviewed in four6view sheets.54PNGs are exact to
already inspected controls,4changed stream stages reviewed in matched close crops (373–474
changed pixels, the estuary crocodiles; the rest of each frame exact), and cave exterior is an
additional current view without a matching previous final-matrix control. No golden was replaced.
`build/campaign-continuation-evidence-audit.json` and `build/campaign-continuation-holistic-{1..4}.png`.
Both isolated software600sample benchmarks pass. Median255.569→258.1093ms(+.994%),
mean269.3887→268.2691,p95346.9588→334.0762,RSS952220→916532KiB(−3.75%).
Tracked scene snapshots AND ranges are identical,153–154draws. These are llvmpipe/LP8 numbers,
not hardware FPS. Final kernel gates empty, private devices and unchanged artifacts verified.

Audit retained the small observed TRex ball-support mismatch for correction rather than closure:
−.0461321..+.0553894m over2439planted samples on its actual100s CPU route. Existing walking
baseline `build/software-captures/20261102T220946.794859Z-trex` captured and visually inspected;
rest/close baselines above are preserved. Narrow SetGround integration and planted slow-step
settling now being built/tested, using existing renderer, route and rig. No new inspection
framework, model, landmark or population. Goal stays ACTIVE until this qualification and the
original criterion ledger are finished.

### Qualified final TRex support correction — 2026-11-02

TRexMotion now uses the existing drawn surface for body, ball and toe heights, installed beside
the deer/elephant/crocodile support. Null/outside-mesh analytic fallback remains; route, rig,
model, behavior, population, geometry/draw budget retained. As with the elephant, below5%pace
a foot labelled planted fully loses its residual swing lift. No new camera or animation engine.
Dedicated route/rest test:2524stance/1578swing samples, max planted error4.76837e−7m, toes above
the visible floor, folded-rest control and fallback pass.

CPU0–3/-j2 Release/OpenGL33 full build passes (`build/campaign-trex-final-build.log`);
**93/93 safe CTests pass in119.53s** (`build/campaign-trex-final-safe-ctest.log`);3contact-focused
tests pass together. Launcher75/75 and validator11/11 remain passing in this source campaign.
Final appc82ce84b121f22f13c6d15f9e3a700dadc7c7746e72e7d8c04426954eb2a4ae2, sameCNA
runtime950faa719ec2fb51cde7220d3e28100f2609731ede56c7cf4088bda0d2dcde5c.

Inspected safe post-fix scopes (all llvmpipe/Pixman/private devices, unchanged artifact, kernel[]):
- `build/software-captures/20261102T221722.511416Z-trex`
- `build/software-captures/20261102T221736.803018Z-trex-close`
- `build/software-captures/20261102T221751.008456Z-trex-rest`
- `build/software-captures/20261102T221805.866659Z-trex-roar`
Walking/rest pairs reviewed in `build/campaign-trex-support-pairs.png`; static camera/clock match.
This is a small contact correction, not an anatomy redesign or dramatic visual upgrade.
Isolated3×20,000updates median21.1901→.660898us/update
(`build/campaign-trex-cpu-comparison.json`), not a97%frame-rate claim. Before CPU binary was
compiled with its matching pre-change header/archive, avoiding an invalid changed-class ABI pair.

Next: final same-binary holistic matrix and software frame measurement, fresh original24section
acceptance ledger, then delivery verification. Goal still ACTIVE until those are actually done.

## Final original-superprompt reassessment after continuation — qualified

Source681f3118; appc82ce84b121f22f13c6d15f9e3a700dadc7c7746e72e7d8c04426954eb2a4ae2;
CNA950faa719ec2fb51cde7220d3e28100f2609731ede56c7cf4088bda0d2dcde5c. Current matching
.cpp/.hpp inventory784files/158779lines (entry780/158427); IslandGame761684bytes vs758372,
CoreTests913532bytes unchanged. Existing user work/content/architecture preserved.

| Original criteria | Reassessment and concrete evidence |
| --- | --- |
|1–4 context, preservation, investigation, safety|Original attachment/README/PLAN/BUGS and incident read; actual baseline/source ownership checked. Focused C++/CNA changes only; no external source/system/driver change or destructive Git operation. All builds CPU0–3/-j2. Verified contained llvmpipe/Pixman, never native AMDGPU. Earlier full acceptance ledger remains supporting evidence, not a substitute for the fresh review.|
|5 forest/vegetation|Earlier correlated ecology, layered forest floor, roots, canopy/palm LOD corrections retained. Current jungle/clearing/ground/stream/beach/overview inspected; grounding and environment/LOD CPU tests pass. No count/polygon brute force or new vegetation engine.|
|6 geology/soil/caves|Earlier rock dual-projection/soil/prop/mine grounding retained. Current headland/ground/cave/cave-top inspected, cave roof remains soil-covered with a thick open mouth. Existing authoritative field and local patches retained; animal support now follows that same drawn surface.|
|7 ocean/coasts/underwater/tides|Nine-wave/warp/filter, foam/caustic/depth corrections retained. Current water/reef-habitat/shipwreck and inspected elephant bathing show depth/shore/underwater integration. Earlier tide-extreme and actual diving qualification remains in the original ledger; no newly claimed physical-water or hardware qualification.|
|8 wildlife movement/habitat|Observed crocodile idle rotation/pedalling fixed, shared four-beat grounded pose and swing, independent water tail. Elephant/TRex drawn body/foot support and slow planted settling fixed;1879/317croc,5498/1710elephant,2524/1578TRex contact samples. Existing bear/deer/boar/goat/bird/fish work retained, representative motion/behavior tests pass; no new species/population.|
|9 resident|Earlier shoulder/arm/waist/face/material corrections retained. Current resident main frame inspected;8motion stages exactly match already reviewed controls, shared pose/shadow tests pass. No new avatar engine.|
|10 lighting/materials/shadows/LOD/weather|Reused SunShadowMap and existing material/LOD architecture. Fresh day,22h castle and thunderstorm/wet hut-eaves review passes, canopy/shadow/fade tests pass. No AO/new shadow engine, global quality increase or disabled LOD.|
|11 landmarks/composition|User-identified UFO/alien/castle repairs preserved and re-inspected in current ufo/alien/castle/gate views. Hut, shipwreck, cave, lighthouse, bunker grounded/readable; meaningful content/positions retained, no new landmarks. Earlier mine/helicopter/volcano/freighter and approach evidence remains in the full ledger.|
|12 traversal/regressions|Existing bunker divider/door/quad/camera and stream/cave fixes retained. Current93safe tests include collision/support/vehicle/course invariants. Prior actual keyboard cave/stream/flight and continuous800frame bunker turn remain cited in the original ledger; those unchanged traversal paths were not falsely described as a newly repeated physical run.|
|13 cost/compatibility|No new triangles/draws/resources; shared terrain cache reduces simulation work. Isolated component medians18.54→17.96us(croc3),12.87→.87us(elephant),21.19→.66us(TRex). Same1280x720/LP8/VSyncOff/120warm/600sample frame pairs; repeated comparable pair167.2766→162.1862ms, stable tracked scene, host variability explicitly investigated. No native FPS/VRAM or untested backend claim.|
|14 maintainability/determinism|Small shared CrocodilePose replaces duplicated renderer/shadow leg math; ground setters follow existing Deer/Croc pattern. Separate targeted tests leave913KB CoreTests unchanged. Valid world fingerprint238be1fec0a0cdd7 preserved; fixed camera/clock/seed evidence deterministic.|
|15 mandatory visual QA|Final same-app24scopes/60PNGs, all24main frames visually reviewed in `build/campaign-final-ground-holistic-{1..4}.png`.58PNGs exact to inspected controls; only3overview/52headland pixels change at the corrected distant TRex, reviewed in `build/campaign-final-ground-aerial-controls.png`.36subsidiary stages exact. No golden updated as proof. Audit `build/campaign-final-ground-evidence-audit.json`, raw manifest `build/campaign-final-ground-holistic-views.json`.|
|16–18 staged implementation/autonomy/quality|Continued beyond each milestone into the next observed defect, committed/pushed each independently qualified stage, then repeated holistic inspection. No feature expansion/rewrite; actual red regressions and before/after imagery guided corrections. Explicit limits below, no claim of perfect anatomy/IK.|
|19 known issues|AMDGPU GPUVM/flight incidents remain OPEN; CNA/Android lifecycle and stock-skinned guest shader/uniform issues remain external/unresolved. Sound was closed by user confirmation and not reopened. Known application motion/reverse-ramp limits remain below.|
|20 holistic integration|Fresh current24scene natural/landmark/wildlife/light/weather review has no newly identified disruptive defect. All source681f3118 available tests pass; current screenshots/renderer/containment/kernel/artifact proofs recorded. Final timing-repeat and same-app elephant/crocodile controls now also pass; no newly identified required implementation remains.|
|21–24 stopping/deliverables/principles|Major observed continuation defects addressed, existing content and stylized identity retained, further rig/terrain expansion offers diminishing returns/risk. Performance-repeat/control records now pass; final documentation commit/push/worktree must be verified before goal completion. No zero-defect/full-platform claim.|

Safe test command: `ctest --test-dir build -LE display -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2`.131registered,93passed,36display-labelled
not run for GPU safety,2orphan validators excluded; `build/campaign-final-ground-test-selection.json`.
No failed test remains. New focused contact tests3/3, launcher75/75 and validator11/11 pass.

Final24scopes all pass actual renderer llvmpipe/OpenGL33, private Pixman compositor and private
devices, unchanged app/runtime content and empty final kernel intervals. This is software
qualification only. All three implementations are pushed in8a92339c,76197fdc,681f3118.
Goal closure is assessed only after this full ledger and the following delivery checks, never merely from a stage build/test pass.

### Final performance investigation and completion of available qualification

Unchanged earlier binary8c914b49bd8d20fc57fe5765a87f896ddab8d1fcc1418d1b5074670e67251b9b
measured255.569ms initially but167.2766ms on repeat: host variability, not code speedup. Final
c82ce84b measured158.7906ms then162.1862ms. This rules out attributing the apparent38%drop to
these tiny support changes. Latest adjacent pair, identical1280x720/LP8/VSyncOff/120warm/600samples:

| Software-only metric | Before | Final | Change |
| --- | ---: | ---: | ---: |
| Median frame ms |167.2766|162.1862|−3.04%|
| Mean frame ms |177.6208|160.9971|−9.36%|
| p95 frame ms |220.7656|175.7886|−20.37%|
| Peak app RSS KiB |886804|883308|−0.39%|
| Tracked draw range |153–154|153–154|same|

Complete tracked scene snapshots and ranges match; geometry/draw/resource budgets unchanged.
No material regression identified in available measurements; no general/native speedup claim.
All five warmed benchmark scopes pass private-device llvmpipe/Pixman, unchanged binary/runtime
and final kernel gates. Raw records `build/campaign-performance/continuation-{elephant-before,
elephant-after,ground-final,ground-repeat-before,ground-repeat-after}`; comparison
`build/campaign-final-ground-performance-repeat.json`. Only the last pair is used for the
current numerical comparison; other measurements retained transparently.

Final same-app motion controls:
- `build/software-captures/20261102T224249.697562Z-elephant-walk`:9/9PNGs exact to the inspected
  ground correction, natural64frame/0.97501mstand→walk sequence and zero planted error.
- `build/software-captures/20261102T224304.272940Z-crocodiles`:1/1PNG exact to the inspected
  swimming control; back/eyes awash, folded legs and tail retained.
Both main frames inspected again; all8elephant stages exact. Those2scopes plus24final holistic
scopes and4TRex controls total **30current-app scopes/74PNGs**. All use appc82ce84b and have
software/private-device/unchanged-artifact/empty-kernel proof. Consolidated record:
`build/campaign-final-ground-delivery-audit.json`; no capture/test/benchmark remains running.

Meaningful retained limits:
- Application: short turning/acceleration stance slip remains without persistent world-foot IK;
  rear-drive reverse-only bunker climbing is weak, while the qualified actual turn/forward exit
  works. No claim of perfect anatomy, all-species/all-time exhaustive coverage or zero defects.
- External CNA/Android: documented lifecycle and stock-skinned guest shader/uniform issues remain;
  dependencies were not patched. CNA here is an extracted ignored source tree without its own
  Git metadata; its parent repository revision is not misreported as a CNA source revision.
- Driver/platform: AMDGPU GPUVM/flight incidents remain OPEN; no physical GPU, current Debug,
  Web/GLES/Android/other backend or stock-fallback qualification. No driver/kernel/firmware change.

All original sections1–24 have been assessed above with actual implementation, reviewed visual
comparisons, safe tests and cost evidence. The initial original campaign improvements, followed
by user-identified UFO/alien/castle and bunker repairs, remain preserved. This continuation added
three further observed contact/motion corrections and completed another whole-island review.
No additional required in-scope implementation is currently identified; further unrelated
content, terrain/rig redesign or unapproved hardware experiments are not warranted by evidence.
Available implementation and safe qualification are complete, with these explicit limits.

Repository: develop; source commits8a92339c,76197fdc,681f3118 already pushed. The following final
commit contains only this acceptance record and the short PLAN update. Its hash, push and clean
origin-synced state are verified after creation and reported directly, avoiding a circular self-hash.
Goal completion is considered only after that final delivery check.

## Active continuation from88a20f5a — 2026-11-03

Original attachment and latest ledger/README/PLAN/BUGS/critical GPU incident revisited; entry
clean develop, previous4commits pushed. NoAGENTS in repository/parent chain. Goal ACTIVE at user
request. Short stages: (1) measured elephant stance/turn slip, matched baseline, existing-rig
foot anchoring and visual/regression qualification; (2) next observed priority, independently
qualified; (3) full original criterion reassessment/current visual matrix and delivery.
Do not close at a stage. Build CPU0–3/-j2, dependencies read-only, software verified/contained.

### Elephant stance/turn baseline and implementation progress

Actual RED `build/campaign-elephant-stance-red.log`:1420support pairs,max50ms slip.0384686m;
stopped turn416support pairs,max slip.0178898m,no swing samples,1.60955rad body rotation.
Baseline source is88a20f5a plus capture-only trace/camera changes, saved app
`build/campaign-elephant-plant-before-binary` and matching static archive/header CPU fixtures.
Inspected matched software baselines:
- `build/software-captures/20261102T232845.827522Z-elephant-turn` (64contacts,8stages,8493triangles)
- `build/software-captures/20261102T232908.649778Z-elephant-walk` (same existing natural transition)
Turn contact sheet `build/campaign-elephant-turn-before.png`. Both pass actual llvmpipe/Pixman,
private devices, unchanged executable and empty final kernel gates. New semantic contact
validator rejects both actual old logs for world-space slip.

Implementation: bounded GroundFootPlant helper stores one world contact and a finite swing to
a fixed landing; completes a swing when the body stops. Existing elephant IK/mesh/shadow reused.
Shorter geared steps during acceleration/pivots, reach-triggered adjustment and2airborne-foot
budget prevent dragging, overextension and loss of support; bathing retains its existing pose.
Initial raw anchoring caused42cm wrist correction (legacy22cm); shortened/bounded steps corrected
it. A preliminary13.4cm result had a failed1-foot support case and is NOT a qualified result.
The corrected support budget measures18.191cm on the same120s route, better than legacy22.043cm;
new regression keeps a20cm ceiling (2cm margin below legacy), zero planted movement and at
least2contacts. Helper tests cover fixed landing/body stopping, paused clock and deferred steps.
First QA build caught misleading indentation in trace output; corrected, retry passes.
No existing production regression/GPU assertion is weakened. Final build/render review pending.

### Qualified elephant world contacts — 2026-11-03

Final app8b1e418dc93c400a907dac7ec50a0d257bc0e6cf828a55a7eb15bae275c2dd14.
Release/OpenGL33 CPU0–3/-j2 full build passes;95/95safe CTests pass94.37s
(`build/campaign-elephant-plant-qualified-safe-ctest.log`),3focused contact tests and76launcher
tests pass. Existing vertex budget8493 retained in every recorded pose, no GPU resource increase.

Matched after scopes, all software/private/unchanged-artifact/kernel[] verified:
- `build/software-captures/20261102T235509.960469Z-elephant-turn`
- `build/software-captures/20261102T235530.443434Z-elephant-walk`
- `build/software-captures/20261102T235547.111437Z-elephant-close`
- `build/software-captures/20261102T235558.377150Z-elephant-bathe`
All8turn+8walk before/after stages visually inspected in
`build/campaign-elephant-plant-{turn,walk}-pairs-{1,2}.png`; fixed camera/seed/clock/background
retained. Turn:141support pairs,zero world slip,1.575rad turn,66airborne-foot samples while
the body is stopped. Walk:213support pairs,zero slip. Clear alternating lift/plant replaces
sliding, with no new distorted limbs. Close/bathe main frames are byte-identical to the already
inspected previous controls; bathing/shape/materials retained. Full integration regression has
zero stance slip,min2support,135stopped-turn swing samples. Final120s reach correction18.191cm
vs22.043cm legacy; the20cm regression ceiling explicitly preserves improvement while enforcing
the corrected2contact budget (the earlier18cm trial budget came from an invalid1contact version).

Isolated3×20,000update median 0.594371→0.681373us per elephant
(`build/campaign-elephant-plant-cpu-comparison.json`). This bounded contact/flight computation
adds only per-animal state, no new geometry/draws; final whole-frame software comparison remains
for integration qualification. All failed intermediate tests retained; no existing assertion
weakened. Next: measure actual remaining biped ball/toe stance slip, capture before if observed,
reuse the small contact helper with the existing rig; goal remains ACTIVE beyond this stage.

### Tyrannosaur ball/toe contact continuation — implementation qualified, integration active

Entry dc7ce3f7; measured actual RED `build/campaign-trex-stance-red.log`:505support pairs,
max50ms ball/toe slip34.37/35.04mm,222stopped-turn support pairs,zero turning swing samples,
1.6311rad body turn. Saved production baseline `build/campaign-trex-plant-before-binary` plus
QA-only trex-turn camera/64world-contact traces; software baseline
`build/software-captures/20261103T000958.272919Z-trex-turn`. Current strict validator rejects
that actual baseline for sliding. Existing navigation, behavior, rig and12212triangle mesh retained.

Reused GroundFootPlant for two world contacts; fixed ball and all3toe tips, foot heading changes
in the return flight, alternating one-airborne-foot budget, short geared pivot/starting steps.
The existing IK and shared visible/shadow state still solve/draw the joints. An initial90cm
half-stance caused339mm metatarsus length distortion versus132mm legacy on the same120s route;
that intermediate render is NOT qualified.60cm still158mm; bounded50cm stance measures66.02mm,
now covered by a140mm regression ceiling. Natural rest/waking smoothly bridges from the last
anchored pose to the existing folded pose, instead of snapping its destination.

New regression initially measured every airborne walk displacement as a rest transition. Its
scope was corrected to frames with lie>0 (threshold unchanged), and now tests both lie-down and
waking. Final `build/campaign-trex-plant-final-stance.log`:372support pairs,zero ball/toe slip,
min1support,121stopped-turn support pairs,86stopped-air samples,1.6311rad turn; max rest/waking
ball displacement175.14mm/100ms and66.02mm metatarsus distortion. No existing assertion weakened.
Five focused support/helper tests pass;77CPU-only launcher tests pass. First full96/96 run
passes182.43s but spanned the final shortened-stride rebuild; a stable-artifact final run follows.

Final turn after `build/software-captures/20261103T003111.587587Z-trex-turn`; all8matched stages
visually inspected in `build/campaign-trex-plant-final-turn-pairs-{1,2}.png`: alternating lift and
plant during an actual stopped Watch turn, no new stretched limb or floating foot. Other static
controls, final safe regression, cost and holistic original-criterion reassessment remain active.
Goal is not closed at this stage. No hardware/external dependency changes; sound remains closed.

Stage delivery: appda57f9bca81b92fd25823136da068be6028083e48b1a80a1ba5c5d253bd09657;
CNA950faa719ec2fb51cde7220d3e28100f2609731ede56c7cf4088bda0d2dcde5c unchanged. Release full
build CPU0–3/-j2 passes. Stable-artifact CTest first process exited143 after45passed tests
(cause not attributed); remaining51passed separately5.34s. Union exactly matches all96safe
tests; no failure hidden or test omitted. Logs/selection/result in
`build/campaign-footplant-final-{safe-ctest,remaining-ctest,test-selection,test-results}.*`.
77launcher and11capture-validator checks pass.36display tests and2orphan validators excluded.

Four final software TRex scopes: turn003111.587587Z,walk003157.052710Z,rest003218.492591Z,
roar003238.824640Z (all20261103T prefixes under build/software-captures). Actual llvmpipe,
private devices/Pixman, unchanged executable and empty final kernel intervals. Final turn has
54support pairs,zero world ball/toe slip,1.386rad rotation,35stopped-air samples. All8matched
turn phases and3static controls actually inspected; control sheet
`build/campaign-trex-plant-static-controls.png`. Existing rest/roar silhouette retained.
sizeof actor1928→2112B(elephant),1824→1936B(TRex); no new heap allocation, mesh or GPU resource.
31scope whole-island/environment/weather/live-bunker review and full-frame software cost pair
continue after this source commit. Goal remains ACTIVE; no stage-level completion claim.

### Holistic review found another real lighting defect — continuation remains ACTIVE

The2fe0790c final broad matrix completed31scopes plus4TRex controls:35scopes/111PNGs,
all verified llvmpipe/Pixman/private devices, unchanged app and empty final kernel intervals.
All35main frames inspected (24in holistic sheets1–4,5animal controls in sheet5,3full-resolution
UFO/alien/castle frames, dawn/bunker directly and4TRex views).98PNGs exactly match previously
reviewed controls, including all17images of the actual800frame bunker turn/forward exit.
Overview/headland differ13/185pixels at the intentionally changed distant animal poses.
Audit `build/campaign-footplant-final-evidence-audit.json`. Legacy trex camera aim follows its
body pose, so its309953changed pixels include a camera-angle change; it is a functional view,
NOT matched-camera improvement proof. The dedicated trex-turn analytic camera provides that proof.

The new06h overview exposed excessive lighthouse glare, so completion was NOT declared.
Baseline `build/software-captures/20261103T004511.364802Z-overview-t06.00` plus
`build/campaign-lighthouse-glare-before-views.json`:06h tower,22h lantern interior/overview.
The old single bright fan had radius up to8.2m and centre shifted1.4m toward the eye; inside
the lantern that offset could cross the camera and its oversized glow washed out the sea/sky.
Actual RED `build/campaign-lighthouse-glare-red.log` reproduces the near-eye geometry invariant.

Local correction: preserve beam motion, night switch, depth read, additive draw and existing
512vertex buffer. Bound halo to2.8m and5.7degree angular radius at its actual shifted distance;
cap offset ahead of the eye and outside the existing36cm lens. Compact bright core/low-energy
radial halo and daylight-adapted halo intensity replace the broad saturated disc.132light
triangles instead of92 (396vertices, still below512); same existing buffer, allocation and draw.
Release build CPU0–3/-j2 passes, focused near-eye regression passes; matched image review pending.
The96previous safe tests retain unchanged binaries/core sources; only the new focused test and
LighthouseRenderer were compiled in this stage. No external source/native GPU changes.

### Lighting attribution corrected; two local defects repaired and visually verified

The first six-view comparison proved the near-eye lighthouse repair: the22h lantern view no
longer has its sea/sky washed out. It also showed the distant overview's large disc unchanged,
so attributing that disc to the lighthouse was wrong. Intermediate records retained in
`build/campaign-lighthouse-glare-intermediate-{views.json,pairs-1.png,pairs-2.png}`; no improvement
was claimed for the still-broken overview. Further code inspection identified the actual cause:
VolcanoRenderer reused its opaque cone's pale fog colour for additive glow. Fog mixed colour
into even zero-energy rims, producing luminous cyan discs and a white streak at distance.

DrawGlow now fades emission toward BLACK and restores the normal opaque fog colour afterwards.
No new draw, triangle, buffer, shader dependency or external patch for this correction. Actual
matched06h overview:6619changed pixels, confined to(290,260)–(395,351);1564near-white pixels
in the crater crop become0, leaving warm lava/steam illumination and an unchanged landscape.
Night overview also loses the false cyan globe; lighthouse beam, daytime tower and22h castle
remain preserved. All6final same-camera pairs inspected in
`build/campaign-lighthouse-glare-pairs-{1,2}.png` and dawn final inspected at full resolution.

Final lighting app4dbb662a7bd970704381e5797a729ffe6bd525076eee74b5a40e13c907c66c80;
manifest `build/campaign-lighthouse-glare-after-views.json` contains6passing software/private/
unchanged-executable/kernel[] scopes.96previous CPU-safe tests plus new near-eye test pass;
only new test/LighthouseRenderer/VolcanoRenderer rebuilt, core and existing96test binaries
unchanged. Additional final day/animal/close-crater controls and comparable cost measurement
continue before the complete original acceptance ledger and goal closure.

Lighting qualification completed before integration delivery:12final-app scopes/28PNGs, all
actual llvmpipe/Pixman/private/unchanged-artifact/kernel[] verified in
`build/campaign-emission-final-evidence-audit.json`. All12main views inspected;22PNGs exact to
reviewed controls (including all9elephant-turn and9TRex-turn images, day forest/water/tower and
night castle). Day overview changes only81pixels at the lava rim; its landscape is unchanged.
Final control sheet `build/campaign-emission-final-controls.png` includes the live near22h crater.
Earlier35scope/111PNG full review is attributed to2fe0790c, not mislabelled as the lighting binary.
Combined retained qualification47scopes/139PNGs, with explicit per-group executable identities.

Cost investigation: original short3x20000TRex timings ramped1.24→.99us and suggested an apparent
speedup; they are retained but NOT used for that claim. Longer warmed ABBA8x200000update runs,
first2passes discarded, no concurrent build/test/capture, measure0.9631295→1.12683us (+17.00%,
absolute+.1637005us). Elephant measured.594371→.681373us (+14.64%,+.087002us). These >10%component
increases are explicit: persistent contacts/finite flights/turning placement cost extra scalar
work for two existing actors. Combined only.000251ms/update and296B fixed actor state; no new
heap or animal mesh/resource/draw budget. The lighthouse adds40small halo triangles/1920B
streamed per active frame within its existing512vertex buffer and one existing draw. Fog fix
adds no geometry/resource. Whole-frame software comparison is still finishing separately.

## Final original-superprompt reassessment — source3a94c887, delivery gate pending

Source commits in this continuation:dc7ce3f7(elephant),2fe0790c(TRex),3a94c887(night emission).
Final app4dbb662a7bd970704381e5797a729ffe6bd525076eee74b5a40e13c907c66c80;
CNA950faa719ec2fb51cde7220d3e28100f2609731ede56c7cf4088bda0d2dcde5c unchanged. Inventory
790CPP/HPP files/159181lines; IslandGame764965B,CoreTests913532B unchanged from the prior stage.
This reassessment covers original sections1–24, with the earlier complete ledger providing
retained implementation/qualification details, and fresh evidence for this continuation below.

| Original criteria | Current assessment and evidence |
| --- | --- |
|1–4 context, preservation, investigation/safety|Attachment,README/PLAN/BUGS,incident and actual branch/worktree/source ownership revisited. Entry88a20f5a clean; all earlier work/content preserved. C++/CNA architecture and authoritative terrain retained. Every build CPU0–3/-j2; dependencies/system/driver source untouched. All graphical workloads use contained verified llvmpipe/Pixman; no physical AMDGPU experiment.|
|5 forest/vegetation|Existing correlated ecology/layering/root/ground pigment/aerial canopy and palm improvements retained. Fresh overview,jungle,clearing,ground,stream,beach reviewed, and final lighting-build jungle exactly matches the reviewed control. World fingerprint238be1fec0a0cdd7 retained; rendered canopy/understory/bush/plant root gaps remain0 in the world report. Existing ecology/grounding/LOD tests pass; no population/polygon brute force.|
|6 terrain/geology/soil/caves|Existing heightfield/local support patches, soil/rock projection/mine/cave-roof work retained. Broad current headland,ground,cave,cave-top reviewed; no open roof/seam newly found. Elephant/TRex feet use the same drawn support as the terrain. Earlier walking/elevated/coast/aerial and cave traversal evidence remains applicable to unchanged terrain/collision code.|
|7 water/coasts/reefs/tides|Existing multiscale wave/foam/caustic/depth and habitat work retained. Broad water,reef,shipwreck and bathing reviewed; final-build water exact to reviewed control. Prior tide-extreme, swimming/diving and shoreline qualification remains recorded; no new fluid simulation or untested hardware claim.|
|8 wildlife movement/habitat|Observed heavy-animal stance/turn drag fixed with finite world contacts and foot orientation, while retaining actual AI/navigation and existing IK/meshes/shadows. Zero elephant/tyrannosaur stance slip; minimum2/1supports, genuine stopped turning steps, bounded rig reach and rest/waking. Earlier crocodile/bear/deer/boar/goat/bird/fish/turtle improvements retained, current representative behavior/support tests pass; no new species.|
|9 resident|Earlier face/sleeve/shoulder/waist/material work retained. Broad current resident and8stages match inspected controls, shared pose/shadow tests pass. Character purpose/rig and route retained; no avatar subsystem replacement.|
|10 lighting/materials/shadows/LOD/weather|Reused SunShadowMap/material/LOD/weather architecture. Fresh dawn/night/clear/thunderstorm scenes reviewed. Integration found and corrected two additional real defects: near-eye lighthouse glare and fog-coloured volcanic additive rims. Matched6lighting pairs plus6day/animal/crater controls reviewed; no new rendering engine/AO or global quality increase.|
|11 landmarks/composition|UFO,alien,castle/gate inspected again in full resolution; prior user-identified geometry/grounding repairs preserved. Hut,shipwreck,cave,lighthouse,bunker and volcanic scenes reviewed, with actual night defects fixed. Prior mine/helicopter/freighter/other landmark evidence retained for unchanged models; no landmark removed/relocated or unrelated content added.|
|12 traversal/regressions|Fresh800frame bunker entry/turn/forward exit succeeds; all17images exactly match the already reviewed continuous traversal, max floor gap.125568m, final depth−5.41552m. Existing cave/stream/swimming/flight keyboard paths and vehicle/collision invariants remain in the earlier ledger and current CPU tests. These unchanged live paths are not falsely described as newly repeated keyboard runs.|
|13 cost/renderer compatibility|Bounded actor state+184B/+112B, extra contact CPU+.087/+.164us explicitly measured/investigated. Animal meshes8493/12212triangles and GPU resources retained. Beacon+40triangles/+1920Bnight stream stays inside the existing512vertex buffer/one draw; black-fog correction adds no geometry. Equivalent whole-frame software pair is recorded separately below; no native FPS/VRAM or other backend claim.|
|14 maintainability/determinism/docs|Small GroundFootPlant and glare policy, focused tests outside913KB CoreTests; no mass refactor, hidden ecology state or dependencies. Fixed-seed world unchanged; helper reset/paused clock/fixed landing/support/rig reach covered. New night work stays outside the giant game file. Stage commits and concise current ledger preserve failure history and real evidence.|
|15 mandatory visual QA|Matched elephant walk/turn and TRex turn baseline/after phases inspected; real rendered contact traces enforce support/slip/visible geometry. Broad2fe0790c:35scopes/111PNGs; final lighting build:12scopes/28PNGs,47retained qualified scopes/139PNGs total with explicit per-group hashes. All mains reviewed;98broad and22final PNGs exact to reviewed controls. Actual dawn/night defects were fixed before accepting integration. No golden update treated as proof.|
|16–18 stages/autonomy/quality|Continued after elephant into TRex, then whole-island review, then observed lighthouse/volcano defects. Each has red evidence, focused code, build/render/review/regression and cost accounting; intermediate failures and wrong initial attribution retained and corrected. No stage alone closes the goal.|
|19 known issues|AMDGPU GPUVM/flight incidents stay OPEN. CNA/Android lifecycle and stock-skinned guest shader/uniform limitations remain external; no dependency patch or silent closure. User-confirmed sound issue stays closed.|
|20 holistic integration|Fresh forest/terrain/coast/reef/landmark/resident/wildlife/weather/lighting/traversal review completed. New lighting errors found by that review were corrected and reviewed again at near/far/day/night views. No additional major in-scope implementation is currently identified; further unrelated content/rig/terrain redesign has disproportionate cost and risk.|
|21–24 stopping/deliverables/principles|Major observed defects addressed; deterministic visual and safe technical evidence documented; existing stylized identity, negative space, structures and exploration preserved. Known limits below remain explicit. Available qualification can close only after cost record and final commit/push/clean-state verification. No zero-defect/full-platform claim.|

Validation:135registered,97CPU-safe tests covered (96stable core/test binaries plus1new glare
regression);36display-labelled tests excluded for GPU safety and2orphan validators excluded.
`build/campaign-emission-final-test-results.json` lists the exact97names and3CTest logs. The
first final96test process returned143 after45passes (unattributed interruption); remaining51
passed separately. New glare test passed separately. No failed or missing test in the union,
and no claim of a single97/97CTest invocation.77launcher and11capture-validator checks pass.
Build logs `build/campaign-{trex-plant-qualified,lighthouse-glare-final,emission-fog}-build.log`.

Visual manifests/audits and contact sheets are linked in the qualified stage sections above.
`build/campaign-footplant-final-world.json` retains the valid world fingerprint and placement/
rendered-ground invariants. All graphics scopes have actual software renderer/private-device/
unchanged-executable/empty-kernel-interval proof; only software validation is claimed.

Meaningful retained limits: reverse-only bunker ramp climbing is weak, while the qualified
actual turn/forward exit works. Procedural animation is stylized; this does not claim perfect
all-species/all-time IK/anatomy or exhaustive defect absence. CNA/Android issues above and
physical AMDGPU qualification remain open. Current Debug/Web/GLES/Android/stock fallback/other
backends are unverified; no driver/kernel/firmware change. Timing on this busy software-renderer
host has substantial variability, so component and full-frame evidence are kept separate.

Final performance figures and delivery verification follow; goal remains ACTIVE until then.

### Final cost record and completion of available implementation/qualification

Same software workload: elephant-close,1280x720,LP8,VSyncOff,120warm-up intervals,600samples;
separate old frozen88a20f5a+QA-only binary versus final3a94c887 app. Source/runtime contents
unchanged across each run; actual llvmpipe/Pixman/private devices and empty kernel intervals.
The complete scene snapshots AND all scene population ranges match exactly between runs.

| Software-only metric | Before | Final | Change |
| --- | ---: | ---: | ---: |
| Median frame ms |330.5149|308.6145|−6.63%|
| Mean frame ms |360.4167|334.5494|−7.18%|
| p95 frame ms |516.6145|481.4472|−6.81%|
| Peak app RSS KiB |967352|957556|−1.01%|
| Tracked draw range |153–154|153–154|unchanged|

Raw `build/campaign-performance/footplant-final-{before,after}` and comparison
`build/campaign-footplant-final-performance.json`. This pair shows no material regression in
this workload; it is NOT a claimed general/native speedup or a tight universal10%bound. The
busy host's software timings differ substantially from the previous campaign's167ms run, and
within-run spread remains large. The small CPU contact increases and bounded night upload/
geometry cost were separately investigated and justified above; no quality increase is hidden
behind changed resolution, renderer thread count, scene complexity or benchmark settings.

All original sections1–24 have now been reassessed with preserved implementation plus fresh
visual/technical/cost evidence. Available in-scope work and safe qualification are complete;
no newly identified required implementation remains. The integration review's newly observed
night defects were repaired, rather than accepting the earlier35scope checkpoint as completion.
Known application/CNA/driver/platform limits remain explicit above; no zero-defect or physical
GPU validation claim. Further unrelated content or rig/terrain redesign is not warranted.

Repository: develop; dc7ce3f7,2fe0790c,3a94c887 pushed. This final documentation-only commit records
acceptance and the short PLAN update. Its hash, successful push, clean worktree and origin sync
are checked AFTER creation and recorded in `build/campaign-stance-final-delivery.json` and the
final response, avoiding a circular self-hash. Goal completion occurs only after that check.

## Active continuation frombde40e70 — 2026-11-03

Entry develop clean/synced; original attachment/README/PLAN/BUGS/incident and latest acceptance
revisited. Previous source/qualification preserved; goal ACTIVE at user request. Stages:
(1) reproduce retained reverse-ramp limit without tuning physics; (2) qualify/fix the observed
lava-lake horizontal light bands with matched close/far views; (3) continue the next observed
priority and reassess all original criteria before delivery/closure. CPU0–3/-j2; no external
source or native GPU experiment. NoAGENTS in repository or parent chain. Sound stays closed.

CPU probe `build/campaign-quad-slope-probe.log`: rear-drive reverse works on flat/10degree ground
(36.31/26.52m uphill in10s). On18degrees it rolls5.37m downhill; estimated rear tyre grip608N
is below788N downslope gravity. Forward18degree climb97.94m succeeds. This supports a physical
rear-axle load/traction limit, not a portal collision defect; no arbitrary reverse-force boost
or drivetrain change is justified. The prior actual turn/forward bunker escape stays qualified.

The inspected22h crater frame from source3a94c887 shows two sharp horizontal brightness bands
across otherwise continuous lava cracks. Actual raw heightfield under the lake stays below5.95m
versus lava18m, so this is not terrain sticking through. Source inspection identifies two large
additive glow billboards that extend beneath the lake and are abruptly depth-clipped by it.
Next: matched baseline close/far captures, bounded billboard height above the existing lake,
near/far/day/night comparison and geometry regression; preserve lake shader, cone and traversal.

### Qualified crater halo clearance; next observed flow-light defect

App22f7c88b42821f9e4f298fd37793c778c9d0c3fb0b0e58c544479aae11202b9c; Release CPU0–3/-j2
build passes,5focused tests pass, full98/98safe CTests pass98.96s
(`build/campaign-crater-glow-safe-ctest.log`). New CraterGlow regression is RED against the
original geometry; corrected half-height keeps the two large halos5cm above the opaque lake
for camera-up projections0..1. Cone/lake shader/support/collision and all geometry/draw/buffer
counts unchanged; only two scalar height bounds per active frame added, no allocation/state.

Matched before/after manifests `build/campaign-crater-halo-{before,after}-views.json`:4views each
(night crater,night overview,night flow,day crater), all verified llvmpipe/Pixman/private devices,
unchanged executable and empty final kernel intervals. All8images actually inspected in
`build/campaign-crater-halo-pairs-{1,2}.png`; night crater also inspected at full resolution.
Original horizontal row-mean jumps32.720/7.008levels at y452/500 are gone; maximum row jump in
the same x460..820,y430..520 lake crop is now2.486levels. Lake cracks/time/camera remain matched;
far warm glow and daytime crater preserved. No golden update used as proof.

Do not stop here: the night flow view exposes separate triangular light bands on the cone,
from camera-facing flow halos intersecting its surface. Next baseline is this qualified flow
image; correct the local light orientation/ground integration with existing additive geometry,
then review near/far/night controls and continue full original acceptance. Goal stays ACTIVE.

### Flow-light surface integration — final candidate rendered, integration still ACTIVE

A first tangent-plane candidate still visibly clipped the cone; retained image
`build/software-captures/20261103T020425.243116Z-volcano-flow-t22.00` and source snapshot
`build/campaign-flow-tangent-candidate.cpp` are rejected intermediates. A uniform ground grid
also failed the dense geometry test(min clearance−52.4mm); concentrating only across the crest
still failed−48.2mm. Grading radial stations near the rounded rim, matching the cone's existing
sampling convention, fixes it without increasing the proposed1800vertices. Final sampled minimum
clearance18.793mm across27,000interior triangle samples; existing geometry/weights/finite/buffer
checks pass. No assertion weakened. CraterGlowTests now covers both observed light defects.

BuildVolcanoFlowGlow creates one fixed surface-following light patch on the authoritative cone;
no new terrain/lighting engine or opaque surface. The old9camera-facing patches(108triangles)
become600surface triangles, +492(<1%of the53644triangle cone). Same existing additive draw and
buffer count; HaloCapacity2048→4096 adds32KiB logical GPU storage, worst current active vertices
3468<4096, so plume/spatter features are retained. Precomputed CPU mesh28,800B and staging+32KiB;
no per-frame surface sampling or new allocation. Active upload grows23,616B/frame. This focused
geometry cost replaces visible intersections; whole-frame before/after measurement remains due.

Release full build CPU0–3/-j2 and5focused tests pass. Full98/98safe CTests pass138.40s
(`build/campaign-flow-ground-safe-ctest.log`);77launcher and11validator tests pass. Six same-app
software views pass in `build/campaign-flow-ground-after-views.json`. All4matched near/far/night/
day comparisons inspected in `build/campaign-flow-ground-pairs-{1,2}.png`; night flow also viewed
at full resolution. Smooth warm light replaces triangular bands; corrected lake and distant
plume remain intact. Two dawn controls and28scene holistic review are next, followed by cost
measurement and full original1–24 reassessment. Goal remains ACTIVE after this second stage.

Other additive/fog audit: hut/fungus/lighthouse/passing-ship use separate glow effects without
opaque fog; balloon disables fog for its burner, shoreline has its own distance-faded glow.
Mine's current visibility gate ends before its fog starts, so no observed pale-fog artifact there.
No speculative fixes were applied to these working paths; selected night controls remain due.

### Flow-light visual/regression qualification completed; cost and delivery remain active

Final candidate app1fce4a062a66ac6d61d0c234413140a7359e558b4ee249250fa795940261dd5a;
CNA950faa719ec2fb51cde7220d3e28100f2609731ede56c7cf4088bda0d2dcde5c unchanged. Full98/98
safe CTests pass138.40s; strengthened finite-coordinate/energy test separately passes without
changing the app. Registry136=98safe+36display exclusions+2orphan validators
(`build/campaign-crater-final-tests.json`). World fingerprint238be1fec0a0cdd7 retained.

Current same-app34software scopes/70PNGs all pass actual llvmpipe/Pixman/private-device/
unchanged-executable/kernel[] gates. All34main views inspected:4local pairs,2dawn controls,
28whole-island and night-light views in `build/campaign-crater-holistic-{1..5}.png`.
63PNGs byte-identical to previously inspected controls, including subsidiary wildlife/resident
stages;7changed/new images all actually reviewed. Night overview differs only272pixels around
the corrected volcanic glow; no model/camera/seed change. Audit
`build/campaign-crater-final-evidence.json`, raw manifests
`build/campaign-{flow-ground-after-views,crater-continuation-holistic}.json`.
The no-longer-clipped flow/lake, near/far/night/day transitions, forest/water/reef, cave roof,
weather, resident/wildlife, alien/UFO/castle and other landmarks are preserved and coherent.

No additional defect found by the broader additive/fog audit or night hut/mine/fungus/lantern
inspection; working paths retained. The documented reverse18degree limitation was measured as
rear-axle grip below downhill load, not patched with a force hack; actual turn/forward escape
and current bunker physics/collision regressions remain valid. Continue through the equivalent
600sample night-flow cost pair and fresh original1–24 acceptance/delivery gates, not another
content expansion. Goal is still ACTIVE.

Performance gate still ACTIVE: first identical1280x720/LP8/22h/120warm/600sample pair has exact
scene snapshots/ranges and105–106draws, but mean140.101→162.711ms(+16.14%),median137.285→147.441ms
(+7.40%),p95166.146→239.399ms(+44.09%). RSS806792→806472KiB. This is NOT accepted as no regression.
Raw `build/campaign-performance/crater-flow-{before,after}` retained; reverse-order after→before
repeat underway with unchanged binaries/settings and no concurrent build/test/capture. Determine
whether the tail increase persists before final acceptance; do not close the goal from images alone.

## Original1–24 reassessment after59160842 — available qualification complete

The previous full campaign ledger remains supporting evidence. This continuation did not stop
at its first stage: it investigated the retained vehicle limit, fixed two observed volcanic
light defects, rejected failed candidates, then reviewed the whole island and other night lights.

| Original criteria | Fresh assessment/evidence |
| --- | --- |
|1–4 context/preservation/safety|Attachment and repository/incident docs revisited; bde40e70 entry clean. Original C++/CNA architecture/content and all user work retained. Every compilation CPU0–3/-j2. Dependencies read-only; actual contained llvmpipe/Pixman, never physical AMDGPU.|
|5 forest/vegetation|Previous ecology/layered forest/ground/root/palm/LOD work retained. Current jungle,clearing,ground,stream,beach and overview inspected; relevant tests pass. World fingerprint238be1fec0a0cdd7 unchanged; no population/geometry increase to vegetation.|
|6 terrain/geology/soil/caves|Authoritative heightfield/local patches and cave roof/soil/rock improvements preserved; current cave/cave-top/headland/ground inspected. Volcano support/cone/lava geometry unchanged; light now clears the lake and follows the same cone rather than intersecting it.|
|7 ocean/coastal/underwater/tides|Current water,reef,shipwreck and coastal views inspected/exact to prior reviewed controls. Earlier multiscale wave,foam/caustic/depth and habitat improvements retained. Tide/swimming/diving qualification and current terrain/water/collision tests remain recorded; no new fluid system.|
|8–9 wildlife/resident|Current bear,crocodile,resident/alien mains and unchanged stages checked against controls; whole98safe suite includes existing contact/behavior/rig regressions. Prior elephant/TRex stance and other representative gait/anatomy fixes preserved; no new species or rig/character engine.|
|10 lighting/material/shadow/LOD/weather|Observed lake bands and slope light triangles corrected; matched near/far/night/day results actually inspected. Hut/mine/foxfire/lantern night review and additive/fog audit find no further observed defect; working paths retained. Existing SunShadowMap/weather/LOD architecture untouched; storm/night/day scenes pass.|
|11 landmarks/composition|Current hut,wreck,cave,lighthouse,UFO,alien,castle/gate,bunker and volcano scenes inspected; prior mine/helicopter/freighter/approach evidence retained for unchanged structures. No meaningful content removed/relocated; two actual landmark light defects fixed.|
|12 traversal/collision|Current bunker entry/turn/escape and volcano climb/rim/flow exclusion CPU tests pass. Reverse-flat/10degree ascent works;18degree reverse rear-axle grip is below downslope load, so no force hack. Prior actual800frame forward-escape/cave/stream/swimming/flight evidence remains applicable to unchanged controllers, not falsely labelled a new keyboard run.|
|13 performance/compatibility|Same buffer/draw count; light-field cost+492triangles,+32KiB GPU capacity,+28,800Bprecomputed CPU mesh,+23,616Bactive upload explicit. Geometry budget/surface tests pass. First mean/tail increase was investigated by the reverse-order repeat below and did not persist. Cost accepted with host-variability bounds; only available OpenGL33software qualification claimed.|
|14 code/determinism/docs|Two small pure geometry helpers and a focused regression outside giant files; no architectural rewrite/dependency/global state. CoreTests913532B and IslandGame764965B unchanged. Current inventory793CPP/HPP files/159283lines. Failed intermediate source/images/metrics retained with exact next steps here.|
|15 visual QA|Final same-app34scopes/70PNGs actual renderer/private/artifact/kernel proofs; all34mains reviewed,63PNGs exact controls. Matched4+4crater and4flow pairs plus full-resolution critical views; no golden update accepted as proof. Audit build/campaign-crater-final-evidence.json, sheets campaign-crater-holistic-{1..5}.png and campaign-{crater-halo,flow-ground}-pairs-{1,2}.png.|
|16–18 staged autonomy/quality|Continued crater→flow→other-night-light audit→whole-world qualification; real failures revised before declaring visual success. Full98safe/77launcher/11validator tests pass. Performance/delivery gates remain active; no milestone-only completion.|
|19 known issues|AMDGPU incident and external CNA/Android lifecycle/stock-skinned issues remain OPEN; no silent workaround/closure or dependency patch. Sound remains user-closed.|
|20 integration|Fresh whole-island/biome/wildlife/landmark/weather/night inspection finds no additional major in-scope correction currently required. New light effects integrate with existing geometry; previous valuable content and exploration preserved.|
|21–24 stopping/deliverables/principles|Implementation/visual/regression/cost criteria pass with the limits below; final delivery is verified after the documentation commit. Further unrelated features or terrain/rig rewrites have no evidence-based need. No zero-defect/full-platform/hardware claim.|

Remaining bounds:18degree reverse climb is traction-limited in the retained rear-drive model,
while actual turn/forward exit works. Stylized procedural anatomy/animation is not an exhaustive
all-species/all-time IK guarantee. Physical AMDGPU and current Debug/Web/GLES/Android/other
backends remain unqualified; CNA/driver issues are external/open. No driver/kernel/system change.

### Performance investigation resolved; final acceptance and delivery gate

Both orders used identical22h volcano-flow,1280x720,LP8,VSyncOff,120warm-up/600sample settings,
verified private-device llvmpipe/Pixman, unchanged executables/runtime and empty kernel intervals.
All4raw scopes retained under `build/campaign-performance/crater-flow-*`.

| Chronological software run | Median ms | Mean ms | p95 ms | Peak app RSS KiB |
| --- | ---: | ---: | ---: | ---: |
| Original before |137.2847|140.1010|166.1458|806792|
| Original after |147.4407|162.7105|239.3989|806472|
| Reverse-order after |105.7140|108.1363|124.8695|802088|
| Reverse-order before |113.2603|115.5100|134.3633|799336|

The initial+16.14%mean/+44.09%p95 was investigated, not discarded. Its application user CPU time
was566.17→575.48s(+1.64%), while granted CPU share513→432% and involuntary context switches
49093→98575 changed substantially. Unchanged final binary median147.44→105.71ms on repeat and
p95239.40→124.87ms show that the initial latency increase is not a stable code-only result.
Scheduler/host contention is an inference supported by these counters, not a proven root cause.

The reverse-order comparable pair(before113.2603→after105.7140ms) has median−6.66%,mean−6.38%,
p95−7.07%,RSS+0.34%. Scene snapshots match, draw range105–106 and all population ranges match;
mean tracked draws105.5500→105.5567 differ by4submissions over600frames as the non-fixed clock
crosses a visibility boundary. This tiny count variation is explicit, not described as exact
range equality. No persistent material regression identified; no general speedup/native FPS
or tight universal10%claim. Bounded extra geometry/storage/upload is justified above by the
removed surface intersections. Final repeat record `build/campaign-crater-performance-repeat.json`;
first pair remains in `build/campaign-crater-final-performance.json`.

All original criteria1–24 have now been reassessed above with preserved campaign work, two new
implemented visual corrections, actual matched before/after review, a new regression, full98safe
tests, broadened integration/night-light inspection and cost investigation. No additional major
required in-scope implementation is identified. Further arbitrary content/terrain/rig expansion
has diminishing return and disproportionate risk. Known physical-model/CNA/driver/platform
bounds remain explicit; no zero-defect or native-GPU validation claim.

Repository: develop;46dc0dc4 and59160842 pushed. The following final documentation-only commit
records acceptance and the short PLAN update. Its push, exact remote/local identity and clean
worktree are checked after creation in `build/campaign-crater-final-delivery.json` and reported
in the final response. Goal completion follows that check, never merely the completed stage.

## Active forest/environment revisit from62a69f28 — 2026-11-03

Entry develop clean/synced, earlier two volcanic fixes/qualification preserved. Original
attachment and latest README/PLAN/BUGS/critical incident revisited; no repository AGENTS.
Goal ACTIVE at renewed user request. Investigate less-reviewed pandanus/wetland/canopy/fallen-log
views and actual environmental transitions, then implement only observed defects, qualify each
stage and continue to the next unfinished priority. Final original1–24 reassessment/delivery
must precede closure. Existing rear-drive18degree limitation already measured; do not repeat
that investigation or alter physics without new evidence. Sound stays user-closed.
All builds CPU0–3/-j2, external source read-only, verified isolated llvmpipe/Pixman graphics only.
Initial baseline manifests: build/campaign-forest-revisit-initial.json and
build/campaign-forest-remaining-before.json; four valid views reviewed, bare canopy rejected in preflight.

### Mist recycling correction — implementation checkpoint

The four extra forest views were inspected (pandanus/wetland/fallen log plus the explicit
canopy LOD threshold view; the latter is a distant silhouette comparison, not a close crown).
Baselines: build/campaign-forest-revisit-initial.json and campaign-forest-remaining-before.json.
The rejected bare canopy command exited CLI preflight2 before any graphical workload.
Actual CPU reproduction: build/campaign-mist-probe-before.log, wetland32.1032m relocation at
opacity .530728→.58, valley87.5295m at .263671→.265222 with retained old terrain height.
MistTransitionTests correctly failed before the correction (campaign-mist-transition-red.log).
Implemented smooth outer-habitat dissolution plus3s formation using existing ten puffs/renderer;
valley recycling now reanchors Y invisibly. Initial established mist remains present; noon
burn-off and deterministic drift retained. New64frame/eight-stage inspection views exercise real
wraps without editing puff state. Fixed existing wetland diagnostic which read the shared
renderer's later valley count. No graphics resources/geometry/population added.
CPU ten-minute tests pass: wetland31wraps/max opacity step.031631; valley8/.0305631, invisible
relocation endpoints, identical twin runs, new-ground support and noon burn-off. Final build,
matching after sequences, full safe suite, performance and renewed holistic review still pending.
Before sequences actually reviewed: build/campaign-mist-{wetland,valley}-before-stages.png;
raw directories 20261103T032948.505830Z-wetland-drift and
20261103T033201.068897Z-valley-drift-t06.50. These are subtle atmospheric discontinuities,
not a new vegetation or particle system. Goal remains ACTIVE; continue qualification/next priority.

### Mist stages visually accepted; proceed through the remaining priorities

Release/OPENGL33 final source builds with CPU0–3/-j2 (campaign-mist-after-build.log).
Safe command `taskset --cpu-list 0,1,2,3 ctest --test-dir build -LE display -E
'^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$' --output-on-failure -j2`
passes100/100 in126.88s (campaign-mist-safe-ctest.log). Registered138:36display tests excluded by
safety and2standalone validators excluded because they need the corresponding graphical fixture;
this is not a pass of those38. Launcher77, image-validator11 and new semantic mist3 Python tests
pass, including opaque teleport/incomplete/nonfinite/stale-clock/wrong-subject rejection.
Matching sequences pass contained llvmpipe/Pixman and live64/64frame accounting; all8stages each
were actually reviewed in campaign-mist-{wetland,valley}-pairs-{1,2}.png. The improvement is a
subtle loss of the previously abrupt bank disappearance, with no geometry/material changes.
Final raw scopes:20261103T033845.810057Z-wetland-drift and
20261103T033916.021700Z-valley-drift-t06.50. Wetland actual wrap frame39/jump32.22965m now
opacity3.0813e-6→0, max live opacity step.020086; valley frame21/87.52947m,
6.8208e-7→0/max.015735. These are actual current live-gust paths, distinct from the CPU probe.
No native GPU qualification; runtime SHA950faa719ec2fb51cde7220d3e28100f2609731ede56c7cf4088bda0d2dcde5c unchanged.
Next: finish41scope holistic matrix (includes earlier34 plus3forest/dawnstream/elephant/TRex/
bunker-turn), inspect all images, compare preserved controls, finish before/after software cost
pair, then rewrite full original1–24 acceptance evidence and verify commit/push/clean delivery.
Goal ACTIVE. No stage-only completion claim.

### Full visual/traversal qualification passed; performance gate OPEN

Source d4fc6ba6 committed/pushed. All41holistic scopes/113PNGs plus2mist sequences/18PNGs were
actually reviewed: campaign-mist-holistic-{1..7}.png, campaign-mist-all-stages-{1..12}.png and
matched pair sheets above. Audit build/campaign-mist-final-evidence.json verifies the same final
app2d9670b09960fe709c1a8d4f5e31c50bb42d8b17be07c823bd177dc62f0ac585, private llvmpipe/Pixman,
unchanged content and zero kernel records in all43scopes.73holistic PNGs are byte-exact controls;
other40 are additional dawn-stream/elephant/TRex/bunker-turn views/stages, not unexplained changes.
World fingerprint238be1fec0a0cdd7 remains unchanged. Actual64frame elephant/TRex contact gates
and800frame bunker turn/forward escape pass; all131final images reviewed. A comment now clarifies
that the inspection's initial-field wind search can shift relative to the live gust clock;
rebuilding it produced the identical app hash (campaign-mist-final-build.log), no behavior change.

Initial same1280x720/LP8/VSyncOff/120warm600sample software pair did NOT pass cost review:
median225.3072→349.4196ms(+55.09%),mean227.0030→373.3687(+64.48%),p95244.0556→507.9353(+108.12%),
RSS802220→907812KiB(+13.16%). Scene snapshots AND all recorded scene ranges are exactly equal.
Application granted CPU411→279%, involuntary switches54180→163681, user CPU710.01→817.39s(+15.12%).
Host scheduling/frequency or renderer queue effects are hypotheses, not a demonstrated cause or
an excuse to accept this result. Raw scopes campaign-performance/mist-wetland-{before,after},
summary campaign-mist-performance.json. One reverse-order repeat after→before is now running
with identical settings/executables/containment. Retain all four results and investigate;
do not close the goal or claim a performance pass before that gate resolves.

## Original1–24 renewed acceptance ledger afterd4fc6ba6

The original attachment/superprompt and the preceding full implementation record remain the
scope. This continuation proceeded from forest review through both mist defects to complete
visual, collision/locomotion and regression qualification. Cost investigation is documented below; final delivery is checked after its commit.

| Criterion | Evidence and bounds |
| --- | --- |
|1 project context|Fresh tracked inventory796CPP/HPP files/159463lines; actual world report valid, fingerprint238be1fec0a0cdd7. Existing content inspected, no inventory blindly recreated.|
|2 absolute constraints|Original C++/CNA/heightfield/rendering/gameplay preserved; entry clean, earlier commits retained. No dependency/system/driver source, resets, merges or new packages. Source d4fc6ba6 pushed.|
|3 GPU safety|Incident documentation read; all43final scopes prove actual llvmpipe/isolated Pixman/private devices/unchanged artifacts/zero kernel records. Physical AMDGPU remains untested/open.|
|4 repository investigation|README/PLAN/BUGS/visual/test/incident records and field/renderer integration inspected; four extra forest baselines and live mist CPU/render reproduction established before edits.|
|5 forest/vegetation|Previous ecological distribution/canopy/layering/moss/root/palm work retained. Fresh jungle/clearing/ground/stream/beach/pandanus/wetland/fallen-log and distant canopy-threshold views reviewed; no added trees/geometry or placement change.|
|6 terrain/geology/soil/caves|Authoritative terrain/local grounding and cave roof retained; current overview/headland/ground/cave/cave-top/volcano inspected. Terrain/support/placement tests pass. No new terrain pipeline.|
|7 water/coast/reef/tides|Previous multiscale waves/foam/caustic/depth/reef work retained; current coast/water/underwater/wreck reviewed, preserved controls exact. Current water/tide/support tests pass; prior live diving proof applies to unchanged controllers, not a claimed fresh keyboard run.|
|8 wildlife|Current bear/crocodile/elephant/TRex live stages reviewed; actual64frame world-contact checks for elephant/TRex and160frame crocodile pass. Earlier habitat/gait/anatomy fixes preserved; no species/population increase.|
|9 resident|Current64frame/eight-stage resident poses/materials/waist reviewed; same body/gait/ground integration preserved. No replacement rig/avatar engine or new character content.|
|10 lighting/material/shadow/LOD/weather|Existing shadow/material/LOD/volcanic fixes remain exact in preserved controls; dawn/day/storm/night/foxfire/hut/mine/lantern reviewed. New mist fades at habitat edges, reforms over3s and reanchors to valley ground invisibly.|
|11 landmark integration|Current hut/wreck/cave/lighthouse/UFO/alien/castle/gate/courtyard/bunker/volcano reviewed. Prior mine/helicopter/freighter placement evidence remains applicable; no landmarks removed or relocated.|
|12 exploration/collision|Fresh800frame actual bunker entry/turn/forward escape with16stages passes; terrain/cave/bunker/vehicle/stream regressions pass. Previously measured18degree reverse rear-drive traction limit retained; forward escape works.|
|13 performance/compatibility|Seven-run investigation below retains both unfavorable pairs and identical-binary controls; software latency remains host-sensitive, with no claimed causal speedup/root cause. Last original-path run versus similar-CPU-share baseline: mean+5.1%,median+10.0%,p95+10.6%,RSS+1.8%. No mesh/buffer/draw-capacity increase;20total mist slots unchanged. Native/other-platform qualification not claimed.|
|14 source health/determinism|Shared small opacity helper and focused tests outside giant CoreTests; initial established mist retained, identical twin trajectories/wraps verified. IslandGame767988B/CoreTests913682B; game grows only bounded QA integration, no architectural refactor.|
|15 visual QA|Matched old/new64frame sequences: identical camera/seed/config and exactly equal X/Z trajectories; old opaque endpoints rejected, new invisible endpoints accepted. Four pair sheets actually inspected; no golden-image update used as proof.|
|16 implementation stages|Preserved A–H campaign changes; continued forest inspection→wetland correction→valley grounding/formation→whole-island/night/weather/locomotion/traversal regression qualification.|
|17 autonomous execution|No routine approvals or milestone stop; d4fc6ba6 committed/pushed while broader qualification continued. Failed CLI preflight and unfavorable cost result retained and investigated.|
|18 quality bar|100safe tests/77launcher/11image-validator/3mist-validator tests pass; actual rendering reviewed. Determinism/function/architecture/scope verified. Cost investigation is accepted with the explicit software-latency limit below, not hidden by changed settings.|
|19 known issues|AMDGPU GPUVM and external CNA/Android lifecycle/stock-skinned issues remain OPEN/read-only. Audio remains explicitly user-closed. No unrelated Android/driver work or silent issue closure.|
|20 final integration|41scopes/113PNGs plus2transition scopes/18PNGs inspected;73holistic PNGs exact to reviewed controls,40 additional viewpoints/stages. Coherent ground/vegetation/water/structures/light retained with smoother temporal mist.|
|21 stopping criteria|Observed atmosphere defects fixed with matching proof and no visual/functional regression found in this matrix. Cost findings and limits are documented below; final push/clean check follows the documentation commit. No stage-only goal completion. Further wholesale modeling/feature expansion has no demonstrated need.|
|22 deliverables|Actual source, tests, baseline/after pair sheets, raw contained captures, current world/test/artifact audit and cost runs retained under build/campaign-mist-*; final response will distinguish implemented changes, tests, performance, limits and repository state.|
|23 development principles|Existing systems/content/negative space preserved; deterministic placement, bounded geometry and readable local C++ favored over expansion/rewrites; safe software evidence only.|
|24 intended outcome|Prior forest/terrain/coast/character/landmark cohesion improvements retained and rechecked; two real atmosphere discontinuities corrected. This supports a coherent stylized island, not a zero-defect/photorealistic/all-platform claim.|

Remaining qualification bounds are explicit: stylized procedural geometry/animation does not
promise exhaustive all-species/all-time IK;18degree reverse slope remains rear-traction limited;
physical AMDGPU and current Debug/Web/GLES/Android/other backends are unqualified. External CNA/
driver issues remain open. These limits do not authorize unsafe tests or unrelated rewrites.

Reverse-order pair also initially fails: before201.3121/208.6842/240.1952ms/RSS802500KiB,
after305.0971/291.9731/347.7399/RSS892752KiB. This was not dismissed as scheduling noise.
Standalone alternating200000-update runs (campaign-mist-cpu-cost.json) measure both fields together
to roughly.03–.05ms/update; field sizes remain416/496B and paths/checksums match. No new graphics
buffer, allocation capacity or per-field draw is introduced. Read-only CNA upload inspection
confirms existing fixed-capacity storage; no demonstrated dependency defect or patch.
A diagnostic relink of final game/render objects with old core runs173.6936/175.1315/192.5074ms,
RSS809376KiB. Then a bitwise-identical final executable copy runs194.5181/196.9733/220.9855ms,
RSS847380KiB/granted433%CPU/26859involuntary switches. These isolate actual code and show the same
final binary varying substantially; they do not prove a filename effect or identify the host
cause. Actual original final path is now running as the last control, same settings and no
concurrent compilation/test/capture work. All unfavorable runs remain in the investigation.

### Final cost review, acceptance and delivery gate

All seven runs use1280x720/LP8/VSyncOff/120warm600samples, actual isolated llvmpipe/Pixman,
unchanged binaries/runtime, zero kernel records, and identical recorded scene snapshots/ranges.
No sample/settings change conceals the two initially unfavorable pairs. The hybrid relink is
an ignored diagnostic artifact, never the delivered application; the renamed copy is bitwise
identical to the final original executable. Raw scopes are under build/campaign-performance/mist-*.

| Chronological software run | Median ms | Mean ms | p95 ms | Peak app RSS KiB | Granted CPU % |
| --- | ---: | ---: | ---: | ---: | ---: |
| Before |225.3072|227.0030|244.0556|802220|411|
| After |349.4196|373.3687|507.9353|907812|279|
| Repeat after |305.0971|291.9731|347.7399|892752|348|
| Repeat before |201.3121|208.6842|240.1952|802500|431|
| Final render objects/old core diagnostic |173.6936|175.1315|192.5074|809376|447|
| Identical final copy |194.5181|196.9733|220.9855|847380|433|
| Actual final original path control |247.8276|238.5693|269.9571|816780|412|

Investigation: two unfavorable pairs are real measurements, not silently discarded. The same
final code ranges194.52–349.42ms median and847380–907812KiB RSS across the first/copy runs;
the final original path is816780KiB. A filename cause was NOT established. Compared with the
original before run at almost equal whole-run CPU share411→412%, actual final path mean is+5.10%,
median+10.00%,p95+10.61%,RSS+1.81%, while total user CPU710.01→690.18s decreases2.79%. Relative to
the reverse-order before baseline, the last control is still+23.11%median/+14.32%mean/+12.39%p95;
that bound is explicit. Scheduling/frequency/renderer queue effects remain an inference; no exact
host cause or causal speedup is claimed. All measurements and checks are in
build/campaign-mist-performance-final-investigation.json; direct field CPU/state measurements in
campaign-mist-cpu-cost.json. Fields retain416/496B and combined measured update cost about.03–.05ms;
max300vertices/100triangles per mist draw, existing shared buffers/two field draws unchanged.

Cost review is concluded with a documented software-latency limitation, not a claim that every
run passes10% or that a driver problem was fixed. The original stopping criterion permits
investigated/documented regressions. No capacity/geometry increase or direct costly application
operation warrants a speculative renderer rewrite; physical GPU performance remains unqualified.
Future causal latency attribution needs a controlled quiet host and separate hardware approval
if it includes hardware tests. Do not rerun an indefinite stress loop to chase a passing number.

All original1–24 criteria have now been reassessed above, with actual implementation, matching
visual proof, complete safe tests, whole-island/motion/traversal review and the cost limits.
No further observed major in-scope defect remains from this review; exhaustive all-time/all-species
or zero-defect guarantees are not asserted. Final source d4fc6ba6 is already pushed; the following
documentation/comment-only commit records this acceptance. Its actual push/remote identity/clean
worktree are verified after creation in build/campaign-mist-final-delivery.json before goal closure.


## Active continuation fromd50ccc9f — 2026-11-03

Entry develop clean, previous mist/visual/cost/delivery proof preserved. Original attachment,
README/PLAN/BUGS/critical incident reviewed; no applicable AGENTS. Max4compilation cores,
read-only CNA/Sharp, verified private-device llvmpipe/Pixman only. Audio stays user-closed.
Stages: inspect less-reviewed mangrove/rock/plane/mine/cottage/windthrow/palm views; implement
observed defects with matching rendered comparisons; continue to the next priority and final
full original-criterion review. Existing7-run latency investigation is retained: do not repeat
unchanged full loops or infer a code cause without new evidence. New render baselines under
build/campaign-coastal-revisit-before.json. Goal ACTIVE; no stage-only completion.

### New observations and first correction

Eight contained baseline views completed in campaign-coastal-revisit-before.json; all inspected.
The rock-contact camera sits downhill at.65m and its view is largely terrain: earlier contact
qualification cannot use this image as visible proof. Added a bounded terrain-ray camera helper
and focused ridge/drawn-terrain tests; selected rock/mesh/placement remain unchanged. After view
under capture. This is explicitly a QA-camera correction, not a matching-camera geometry claim.
Fallen-palm CPU probe (campaign-fallen-probe.log) finds the first curved stem supported within4cm,
with a natural arch up to40cm; not evidence of a whole floating trunk. Other crowns reach66cm,
but the existing dried fronds connect them to ground. Do not flatten every curved stem or rewrite
its placement from an ambiguous view. Existing pose/foliage/collision are retained pending a
stronger defect. The keeper cottage is authored rough stone but its surfaces read as broad plain
grey panels; inspect reuse of the existing filtered masonry receiver without changing geometry.

Rock camera stage accepted: bounded ray helper clears synthetic ridge and drawn-terrain probes;
IslandGroundInspectionCameraTests passes. After20261103T051554.858773Z-rock-contact actually
reviewed full resolution: rock/soil contact now visible with no new gap; no rock geometry change.
Build CPU0–3/-j2 and contained llvmpipe/Pixman capture pass. Continue directly to cottage material:
reuse filtered CastleStone receiver for only the cottage range, retain plain tower/wood/rubble,
original geometry/shadows/collision and stock fallback. Matching baseline cottage already saved.

### Cottage material implementation/first visual review

Existing filtered CastleStone receiver reused only for the cottage's original3828-vertex range;
1296vertices tagged plain preserve timber/soot/loose rubble. Main tower remains in the original
plain receiver; same vertex buffer and13850total triangles, no placement/collision changes.
Only the visible cottage range adds one opaque draw; conservative separate cottage bounds avoid
its submission from tower-only views. Stock/plain fallback retains the original single draw.
Initial full-resolution daytime material view reviewed: readable masonry replaces smooth grey
panels while broken silhouette/moss/wood remain intact. All101safe tests pass in101.47s; focused
camera test included. Final lighting comparisons/culling controls and one before/after software
cost pair are in progress. Continue to previously less-reviewed lying/working poses next.

Cottage material accepted visually in four matching day/dawn/night/storm pairs, actually reviewed
in campaign-cottage-pairs-{1,2}.png. Difference boxes451,167–948,521 stay on the cottage; outside
pixels identical, and conservative culling preserves all four full images byte-for-byte.
Same13850triangles; only37722–41549vertex range uses masonry (1296plain tagged vertices retained).
Exploratory old-clock profile82.72→66.41ms median is NOT a causal speedup claim: before/after
recorded draw ranges differ because named benchmarks below100ms still use wall-clock simulation;
tracked counts also omitted lighthouse submissions. Those are confirmed qualification defects,
not a newly established explanation for the prior seven wetland timings (all slower than100ms).
Next stage now implements a common fixed inspection simulation step, actual update/time report
and explicit lighthouse counts; both plain and masonry cost references must share that correction.
