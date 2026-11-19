# 05 — Technology

## Constraints
- **Language:** C17 (no C++ in the game; tools may use Python for scripts).
- **Platform layer:** SDL3 (window, input, gamepads, touch, audio, files,
  threads, time).
- **Graphics:** OpenGL 3.3 core (desktop) / OpenGL ES 3.0 / WebGL 2. One code
  path: we write against the **common subset** of GL 3.3 and GLES 3.0; shaders
  are written once with a small preprocessor header that sets the `#version`
  and precision lines.
- **Targets:** Windows (x64), Linux (x64), Web (Emscripten → WebAssembly +
  WebGL 2), WebXR (immersive-vr in the browser).
- **Build:** CMake ≥ 3.25 with presets (`linux-debug`, `linux-coverage`,
  `windows-msvc`, `web-release`, …); dependencies vendored or via
  `FetchContent` with pinned versions.

## Architecture

```
          ┌──────────────────────────────────────────────────────┐
          │                      app / main                      │
          └────────┬───────────────────────────────┬─────────────┘
                   │                               │
     ┌─────────────▼────────────┐     ┌────────────▼────────────┐
     │  game (modes, hall,      │     │  platform (SDL3 glue,   │
     │  calendar, save, menus,  │     │  GL context, Emscripten │
     │  level scripting)        │     │  & WebXR glue, files)   │
     └──┬──────────┬─────────┬──┘     └────────────┬────────────┘
        │          │         │                     │ (thin, ≤5 % LoC)
  ┌─────▼───┐ ┌────▼────┐ ┌──▼──────┐  ┌───────────▼───────────┐
  │  sim    │ │ render  │ │  net    │  │ audio (mixer is pure, │
  │ (pure,  │ │ (API +  │ │ (proto, │  │ output via SDL3)      │
  │ determ.)│ │ GL back)│ │ snaps)  │  └───────────────────────┘
  └────┬────┘ └────┬────┘ └────┬────┘
       └───────────┴───────────┴──────► core (math, memory, containers,
                                         rng, hashing, log, serialisation)
```

- **core/**: math (with cglm), arenas and pools, containers, PCG random,
  hashing, logging, binary serialisation. Pure, no I/O.
- **sim/**: entities, physics and collision, AI, weapons, the **mechanic
  modules** (see [core gameplay](03-core-gameplay.md#mechanic-modules)),
  level logic. **Pure and deterministic**: a fixed time-step (60 Hz), no
  wall-clock, no globals; all randomness through seeded RNG. Sim state is
  serialisable (for saves, networking and replays).
- **render/**: a renderer API (scene, camera, lights, meshes, sprites, voxels,
  terrain, post) with the GL backend behind it. The CPU-side parts (culling,
  sorting, voxel meshing, sprite atlas packing, LUT building) are pure and
  unit-tested; the GL calls are a thin backend.
- **audio/**: a pure software mixer (tested) feeding an SDL3 audio stream.
- **net/**: protocol, serialisation, snapshots, delta compression, client
  prediction & interpolation (pure) plus transports (platform).
- **game/**: game modes, the hall, the **calendar**, saves, menus, the
  level-script host.
- **platform/**: the only code that talks to SDL3, OpenGL context creation,
  Emscripten and WebXR. Kept thin.

### Data-oriented entities
A simple ECS-like design: entities are IDs with generation counters;
components live in dense arrays; systems are plain functions that iterate.
No dynamic allocation per frame (arenas reset per frame).

### Level scripting
- Most level logic is wired in the editor with **entity I/O** (Quake-style
  `target`/`targetname` plus inputs/outputs: `OnTrigger → door.Open`).
- Level-specific choreography (briefings, boss phases, scripted scenes) is in
  small **Lua 5.4** scripts, sandboxed (no `io`/`os`), with a narrow API to the
  sim. Each script is covered by that level's automated game tests.
- Reusable mechanics are **C modules** (unit-tested), never Lua.

## Renderer

### Pipeline (per view)
1. **Shadow pass**: directional shadow map (1024², stabilised), up to 4
   point-light cube shadows (128² per face, in an atlas), updated on change
   or round-robin.
2. **G-buffer pass at internal resolution** (e.g. 480×270): albedo (palette
   index or RGB), normal, emissive/material. At this resolution a deferred
   G-buffer is cheap even on phones, and it makes dozens of dynamic lights
   affordable.
3. **Lighting pass**: tiled light culling on the CPU (tiles of 16×16 px at
   internal resolution), one full-screen pass; lightmaps added where present.
4. **Forward pass**: transparent things (particles, water, glass, portals).
5. **Post**: fog, palette quantisation via 3D LUT + Bayer dithering,
   optional CRT; then nearest-neighbour upscale to the window.
6. **UI** at UI resolution.

WebGL 2 supports the multiple render targets this needs; on devices where
float textures or MRT are slow, a **forward fallback** (max 8 lights per
object) is used automatically.

### Special features
- **Planar mirrors and reflective floors** (day 1, 19, 22) by rendering a
  mirrored low-res view.
- **Portals** in the hall: render-to-texture previews, one updated per frame.
- **Voxel meshing**: greedy meshing per 16³ chunk, ambient occlusion baked into
  vertices; re-meshed incrementally on destruction.
- **Sprites**: 8-angle billboards with optional normal maps; palette swaps for
  team colours.
- **Terrain**: chunked heightfield with LOD; GPU-side updates on deformation.
- **GPU instancing** for large unit counts (day 23) and particles (snow).
- **Procedural textures** (fire, water, plasma) generated on the CPU into
  small textures, like 1998 did it.

### Performance targets
| Target | Device class | Goal |
|---|---|---|
| Desktop | 2018 integrated GPU (Intel UHD 620) | 60 fps at *Classic* |
| Web desktop | same, Chrome/Firefox | 60 fps |
| Web mobile | mid-range 2022 phone (WebGL 2) | 30–60 fps, dynamic resolution, fewer shadows |
| WebXR | Meta Quest 3 browser | 72 fps, reduced effects |

## Content pipeline
```
assets/src/maps/*.map  (TrenchBroom)  ─┐
assets/src/vox/*.vox   (MagicaVoxel)  ─┤
assets/src/sprites/*.png (Aseprite)   ─┼─► tools/tbcook ─► build/content/level_NN.pak
assets/src/models/*.gltf (Blender)    ─┤                   build/content/hall.pak
assets/src/audio/*.ogg|.it            ─┘
```
- `tbcook` (C, part of the repo, unit-tested) compiles maps (brushes → convex
  hulls → render meshes + collision + BSP/portal visibility), bakes
  lightmaps (simple CPU baker), greedy-meshes voxels, packs sprite atlases,
  converts palettes into LUTs and writes `.pak` archives (zip-like, with an
  index and checksums).
- A **TrenchBroom game configuration** + `.fgd` entity definitions are in
  `tools/trenchbroom/`.
- Every level is one `.pak`, so it can be delivered on its day.

## Saves & settings
- Desktop: `SDL_GetPrefPath("AdventOfLAN", "xmas2026")`.
- Web: Emscripten's IDBFS (IndexedDB), synced after every save.
- JSON for settings, binary (versioned, checksummed) for saves.

## Dependencies (all permissive)
SDL3 (zlib), cglm (MIT), cgltf (MIT), stb_* (MIT/PD), Lua 5.4 (MIT), cJSON
(MIT), ENet (MIT, desktop LAN), Unity test framework (MIT). See `CREDITS.md`.

## Repository layout (planned)
```
src/{core,sim,render,audio,net,game,platform,app}/
tests/unit/            # Unity tests, one file per module
tests/game/            # replays, bot runs, invariants, render goldens
tests/web/             # Playwright tests for the web build
tools/tbcook/          # content compiler
tools/relay/           # tiny lobby/relay server for online play
tools/trenchbroom/     # editor configuration
assets/src/ assets/ATTRIBUTION.md
web/                   # HTML shell, touch UI overlay, WebXR glue (JS)
docs/ openspec/
```
