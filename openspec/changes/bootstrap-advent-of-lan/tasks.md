# Tasks

## 1. Repository & CI foundation

- [x] 1.1 Add CMake project with presets (`linux-debug`, `linux-coverage`, `windows-msvc`, `web-release`) and pinned dependencies (SDL3, cglm, cgltf, stb, Lua, cJSON, ENet, Unity)
- [x] 1.2 Add `.clang-format`, `.clang-tidy`, warnings-as-errors
- [x] 1.3 Add GitHub Actions: spec validation, lint, Linux (gcc/clang + sanitisers), Windows MSVC, Emscripten build
- [x] 1.4 Add coverage job with gcovr thresholds (95 % lines / 90 % branches) and the platform-exclusion ratio check (< 5 %)
- [x] 1.5 Add the spec-trace check (every scenario referenced by a test)
- [x] 1.6 Add the gh-pages deploy job (main only, after all gates) and the build hash on the start screen
- [x] 1.7 Add `assets/ATTRIBUTION.md` and the attribution CI check; add `CONTRIBUTING.md` (MIT code / CC BY 4.0 art)
- [x] 1.8 Tests: a trivial unit test and coverage report run green in CI; deploy publishes a "hello" web build

## 2. Core & platform

- [x] 2.1 `core`: math, arenas/pools, containers, PCG RNG, hashing, logging, binary serialisation
- [x] 2.2 `platform`: SDL3 window + GL context (desktop GL 3.3 / GLES 3.0 / WebGL 2), file access, time, pref path / IDBFS
- [x] 2.3 Main loop with fixed 60 Hz sim step and interpolated rendering; `emscripten_set_main_loop` on web
- [x] 2.4 Headless mode (null renderer, null audio)
- [x] 2.5 Tests: unit tests for all `core` modules; headless smoke test

## 3. Simulation & player core

- [x] 3.1 Entity storage, components, systems
- [x] 3.2 Collision (brush hulls; box hull for players, see design 12), player movement (walk, jump, crouch, sprint, step-up, air control)
- [x] 3.3 Health/armour, damage, difficulty scaling, checkpoints, death/respawn, co-op revive
- [x] 3.4 Weapons framework (hitscan, projectile, melee, charge); base kit (Candy Cane, Snowball Blaster, NOEL-Pad)
- [x] 3.5 Inventory with loaners and level-scoped removal; collectibles (bauble, floppies, challenge)
- [x] 3.6 Replay recording/playback with state checksums
- [ ] 3.7 Tests: unit tests for every `player-core` scenario; determinism replay test

## 4. Calendar, saves & settings

- [x] 4.1 `game/calendar`: pure open-portal function (local date, server time, persisted unlocks, playtest/date flags)
- [x] 4.2 Save system (versioned, checksummed, backup on corruption) and settings (JSON)
- [x] 4.3 Tests: table-driven unit tests for every `advent-calendar` scenario incl. time-zone and DST edges; corrupt-save test

## 5. Renderer

- [x] 5.1 Low-res render target with integer nearest-neighbour upscale and letterboxing (resolution presets, split-screen rectangles)
- [x] 5.1b UI layer at 2x internal resolution (own 5x7 pixel font, HUD, briefing and results panels, playtest overlay)
- [x] 5.2 Forward lighting with up to 16 point lights per view, shadowed directional (moon) light, fog (see design 15)
- [ ] 5.2b Point/spot light shadows (cube atlas), deferred G-buffer path for many lights, automatic forward fallback
- [x] 5.3 Palette LUT quantisation and Bayer dithering (runtime-switchable)
- [ ] 5.3b Optional CRT and affine-wobble filters
- [x] 5.4 Brush polygonizer and greedy voxel mesher (CPU), dynamic boxes for entities
- [ ] 5.4b Sprites (8-direction, normal maps, palette swaps), voxel models in the renderer (+ destruction re-meshing), glTF skeletal + vertex-morph models, heightfield terrain
- [ ] 5.5 Mirrors/planar reflections; portal render-to-texture previews
- [x] 5.6 Tests: unit tests for layout, palette/LUT, meshes, camera, scene; GL renderer tested with a real context (Xvfb + Mesa)
- [ ] 5.6b llvmpipe render goldens for test scenes (perceptual diff)

## 6. Content pipeline

- [ ] 6.1 `tbcook`: `.map` compiler (brushes, entities, entity I/O, collision, visibility), lightmap baker, `.vox` import, sprite atlas, palettes, `.pak` writer
- [ ] 6.2 TrenchBroom game config and `.fgd`
- [ ] 6.3 Level manifest schema and validation (mechanic count, resource bars, input schema, trophy)
- [ ] 6.4 Level invariant checker (key/lock order, reachability of bauble and floppies)
- [ ] 6.5 Tests: unit tests for compiler stages; fuzz targets for `.map`/`.vox`/`.pak` readers

## 7. Input

- [ ] 7.1 Action map with per-level input schemas; rebinding and persistence (keyboard/mouse default bindings and mouse look exist: app/controls)
- [ ] 7.2 Gamepad support and aim assist (host-controlled in versus)
- [ ] 7.3 Web touch overlay: auto-switching, floating stick, look area, contextual buttons, gyro, safe areas, left-handed layout
- [ ] 7.4 Tests: unit tests for action mapping, switching logic and aim assist; Playwright mobile-emulation test

## 8. The Hall

- [x] 8.1 Greybox hall built in code: doors 1-24 (two floors, stairs, balcony), voxel tree, NOEL's desk, trophy wall, jukebox, cocoa bar, LAN corner, arena wing
- [ ] 8.2 Portal states bound to the calendar: closed doors block and show a countdown, open doors glow, today's door is gold with NEW (done); live render-to-texture previews (open, needs 5.5)
- [x] 8.3 Tree progress (one bauble and one ring of lights per finished level), act-dependent mood, restored state with top ornament
- [ ] 8.4 Snowball Arena with bots (room exists; bots and match rules open)
- [x] 8.5 Level entry/exit flow: portal -> briefing -> level -> results -> back in front of the door; progress saved; levels are a placeholder test course until group 9
- [ ] 8.5b Memory album at NOEL's desk; trophy wall interaction
- [ ] 8.6 Tests: game tests for every `festive-hall` scenario (5 of 9 done: finding a door, return after finishing, three baubles, shutdown act, restored hall); phone performance check

## 9. Level framework & first levels

- [x] 9.1 Lua sandbox and level-script API; entity I/O runtime
- [x] 9.2 Golden-path bot framework (waypoint scripts, mechanic actions)
- [ ] 9.2b Navmesh for bots and the reachability invariants (Unreachable secret)
- [ ] 9.3 Mechanic module skeletons with tests: `status_bars`, `vehicle`, `heightfield`, `economy`, `squad`
- [ ] 9.4 Personalisation pack loader (local file / URL), defaults
- [x] 9.5 Create and implement change `add-level-01-baublegum-city`
- [ ] 9.5b Create changes `add-level-02-sky-sled`, `add-level-03-operation-tinselium` from the concept docs
- [ ] 9.6 Tests: framework scenarios from `level-framework` and `meta-story`

## 10. Multiplayer

- [ ] 10.1 Protocol, snapshots, delta compression, input bundling; loopback transport with latency/loss simulation
- [ ] 10.2 Client prediction, reconciliation, interpolation; host lag compensation
- [ ] 10.3 Local split screen (2–4) with per-view resolution scaling
- [ ] 10.4 Sleigh Relay (`tools/relay/`), room codes, invite links, WebSocket transport (web + desktop)
- [ ] 10.5 ENet LAN transport with discovery (desktop)
- [ ] 10.6 Co-op progress rules, host-leave handling, version check
- [ ] 10.7 Tests: unit tests for protocol/snapshots/prediction; loopback network game tests; packet fuzzing; Playwright two-context join

## 11. WebXR

- [ ] 11.1 JS WebXR wrapper + C interface (views, projections, controller poses, framebuffer)
- [ ] 11.2 Stereo rendering path, XR quality preset, filter rules
- [ ] 11.3 XR controls, wrist NOEL-Pad, snap/smooth turn, vignette, teleport in the hall, comfort ratings and warnings
- [ ] 11.4 Tests: unit tests for XR input mapping; Playwright WebXR-emulation smoke test

## 12. Release readiness for 1 December

- [ ] 12.1 Phone, desktop-web and native playtests of hall + levels 1–3
- [ ] 12.2 Credits screen with attributions, "inspired by" list and disclaimer
- [ ] 12.3 Verify all CI gates green and gh-pages updated; announce the link to the friends
