# Directional sun shadows

Since 2026-10-19 the island has one real directional shadow map (`SunShadowMap`,
`game/src/SunShadowMap.cpp`) in addition to the older grounding cues below.

- One orthographic light around the camera: a 144 m square (`Radius` 72 m) whose anchor snaps to
  an 8 m grid, so the map is re-rendered only when the camera leaves a cell. Every caster is
  static, which is what makes that amortisation honest.
- Casters: canopy trees, standing snags, understory trees and bushes (far-LOD meshes), living and
  broken palms, the hut, the shipwreck hull and the opaque helicopter airframe. Ground plants,
  grass and animals do not cast.
- Receivers: terrain (including the cave and stream replacement patches), grass, every jungle
  plant surface and palms. Each receiver multiplies by `mix(uCanopyAmbient, 1, visibility)`, the
  same tone the baked canopy field already used, so shade colour is unchanged; the baked field is
  reduced to 45% of its former darkening while the map is active and carries only sky occlusion.
- Portability: depth is packed into an RGBA8 colour target (`DepthFormat::Depth24` for the pass
  itself) and read with an ordinary `sampler2D`, four point taps blended bilinearly; the receiver
  snippet honours EasyGL's `uRtFlipV` render-target row-order flag on unit 3. No depth-texture,
  comparison-sampler or extension is required on desktop GL, WebGL 2 or GLES 3.
- Resolution 2048 px on desktop, 1024 px on Web and Android. Depth bias 0.0014 of the 330 m light
  range (about 0.45 m); edges fade over the outer 6% of the box.
- When the map is active the three projected structure footprints below are switched off
  (`ContactShadowRenderer::SetStructureShadowsEnabled(false)`); the small contact patches and the
  animal grounding shadows remain.
- The debug inventory line reports `sun shadow map <renders> renders, <casters> casters`; a startup
  diagnostic names the resolution and coverage, and each caster family logs if its depth shader
  fails to compile, in which case that family simply stops casting.

Known limits: animals, the hut's and wreck's own surfaces and the water do not receive the map;
rotor blades and the glass canopy do not cast; the map covers 72 m around the camera only.

---

# Bounded large-object sun shadows (2026-10-15, retained as fallback)


Lighting item 11 uses a deliberately portable fixed-day solution rather than a general shadow-map
engine. The island has one permanent sun direction, while CNA targets desktop GL, WebGL2 and GLES3;
a depth render target, extra scene pass and platform-specific sampling fallback would be a large
cost for three static landmarks. The existing terrain-conforming contact-shadow batch already
provides alpha blending, culling, quality-distance scaling and one portable draw.

The batch now contains one broad projected-mass footprint for each of the shipwreck, inland hut and
grounded helicopter. Each footprint starts from the object's actual authored dimensions and yaw.
Its centre moves opposite the fixed horizontal sun by half the height-derived projection length,
and its two radii expand modestly to cover the union between base and projected top. Every vertex
samples authoritative terrain height, so shadows follow hills and the wreck's shallow seabed.
They use low centre alpha and a transparent twelve-segment edge, retaining the smaller existing
contact patches beneath supports. Palm and broadleaf crown shade remains in the shared baked canopy
transmission field; this stage adds no texture, render target, renderer or draw call.

Evidence:

- native llvmpipe `20261015T014613.722242Z-helicopter` passes at 1280x720 with 305 colour bins,
  55.67 luminance deviation and 234.94 spatial contrast; the projected fuselage mass was inspected;
- native hut `20261015T014214.728276Z-hut` and shipwreck
  `20261015T014411.416135Z-shipwreck` were also inspected for restrained terrain/seabed integration;
- Chrome/SwiftShader `20261015T014752.722690Z-chrome-helicopter` passes exterior and interior views,
  60 finite positive intervals per view, GL0, live contexts, distinct frames and clean kernel
  monitoring; the exterior shadow was inspected;
- ARM64 assemble/lint passes without emulator/KVM (24,088,611-byte APK, SHA-256
  `b97097bfd39510ab0a679d7ba8b8f52a9f43ce2ce1c80d0b8a03cbe55a66e52d`, 19,046,040-byte
  `libmain.so`).
- the filtered non-display regression passes 60/60; five display-producing tests remain disabled.

This is bounded mass projection, not leaf-exact, rotor-blade-exact or self-shadowing. That limitation
is intentional and named; it avoids presenting a contact ellipse as a full dynamic shadow map.
