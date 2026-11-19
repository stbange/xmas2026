# Tasks

## 1. Repository & CI foundation

- [ ] 1.1 Add CMake project with presets (`linux-debug`, `linux-coverage`, `windows-msvc`, `web-release`) and pinned dependencies (SDL3, cglm, cgltf, stb, Lua, cJSON, ENet, Unity)
- [ ] 1.2 Add `.clang-format`, `.clang-tidy`, warnings-as-errors
- [ ] 1.3 Add GitHub Actions: spec validation, lint, Linux (gcc/clang + sanitisers), Windows MSVC, Emscripten build
- [ ] 1.4 Add coverage job with gcovr thresholds (95 % lines / 90 % branches) and the platform-exclusion ratio check (< 5 %)
- [ ] 1.5 Add the spec-trace check (every scenario referenced by a test)
- [ ] 1.6 Add the gh-pages deploy job (main only, after all gates) and the build hash on the start screen
- [ ] 1.7 Add `assets/ATTRIBUTION.md` and the attribution CI check; add `CONTRIBUTING.md` (MIT code / CC BY 4.0 art)
- [ ] 1.8 Tests: a trivial unit test and coverage report run green in CI; deploy publishes a "hello" web build

## 2. Core & platform

- [ ] 2.1 `core`: math, arenas/pools, containers, PCG RNG, hashing, logging, binary serialisation
- [ ] 2.2 `platform`: SDL3 window + GL context (desktop GL 3.3 / GLES 3.0 / WebGL 2), file access, time, pref path / IDBFS
- [ ] 2.3 Main loop with fixed 60 Hz sim step and interpolated rendering; `emscripten_set_main_loop` on web
- [ ] 2.4 Headless mode (null renderer, null audio)
- [ ] 2.5 Tests: unit tests for all `core` modules; headless smoke test

## 3. Simulation & player core

- [ ] 3.1 Entity storage, components, systems
- [ ] 3.2 Collision (brush hulls, capsules), player movement (walk, jump, crouch, sprint, step-up, air control)
- [ ] 3.3 Health/armour, damage, difficulty scaling, checkpoints, death/respawn, co-op revive
- [ ] 3.4 Weapons framework (hitscan, projectile, melee, charge); base kit (Candy Cane, Snowball Blaster, NOEL-Pad)
- [ ] 3.5 Inventory with loaners and level-scoped removal; collectibles (bauble, floppies, challenge)
- [ ] 3.6 Replay recording/playback with state checksums
- [ ] 3.7 Tests: unit tests for every `player-core` scenario; determinism replay test

## 4. Calendar, saves & settings

- [ ] 4.1 `game/calendar`: pure open-portal function (local date, server time, persisted unlocks, playtest/date flags)
- [ ] 4.2 Save system (versioned, checksummed, backup on corruption) and settings (JSON)
- [ ] 4.3 Tests: table-driven unit tests for every `advent-calendar` scenario incl. time-zone and DST edges; corrupt-save test

## 5. Renderer

- [ ] 5.1 Low-res render target, nearest upscale, UI layer at 2× internal resolution
- [ ] 5.2 G-buffer + tiled lighting; forward fallback; directional + point/spot shadows
- [ ] 5.3 Palette LUT quantisation, Bayer dithering, fog; optional CRT/wobble
- [ ] 5.4 Sprites (8-direction, normal maps, palette swaps), voxel greedy mesher (+ destruction re-meshing), glTF skeletal + vertex-morph models, heightfield terrain
- [ ] 5.5 Mirrors/planar reflections; portal render-to-texture previews
- [ ] 5.6 Tests: unit tests for culling, tile binning, mesher, atlas packer, LUT builder; llvmpipe render goldens for test scenes

## 6. Content pipeline

- [ ] 6.1 `tbcook`: `.map` compiler (brushes, entities, entity I/O, collision, visibility), lightmap baker, `.vox` import, sprite atlas, palettes, `.pak` writer
- [ ] 6.2 TrenchBroom game config and `.fgd`
- [ ] 6.3 Level manifest schema and validation (mechanic count, resource bars, input schema, trophy)
- [ ] 6.4 Level invariant checker (key/lock order, reachability of bauble and floppies)
- [ ] 6.5 Tests: unit tests for compiler stages; fuzz targets for `.map`/`.vox`/`.pak` readers

## 7. Input

- [ ] 7.1 Action map with per-level input schemas; rebinding and persistence
- [ ] 7.2 Gamepad support and aim assist (host-controlled in versus)
- [ ] 7.3 Web touch overlay: auto-switching, floating stick, look area, contextual buttons, gyro, safe areas, left-handed layout
- [ ] 7.4 Tests: unit tests for action mapping, switching logic and aim assist; Playwright mobile-emulation test

## 8. The Hall

- [ ] 8.1 Greybox hall: doors 1–24 (two floors), tree, NOEL's desk, trophy wall, jukebox, cocoa bar, LAN corner, arena
- [ ] 8.2 Portal states (closed with countdown, open with preview, NEW tag) bound to the calendar
- [ ] 8.3 Tree progress (baubles, light rings), act-dependent mood, restored state
- [ ] 8.4 Snowball Arena with bots
- [ ] 8.5 Level entry/exit flow, briefing player, results screen, memory album
- [ ] 8.6 Tests: game tests for every `festive-hall` scenario; phone performance check (manual + recorded)

## 9. Level framework & first levels

- [ ] 9.1 Lua sandbox and level-script API; entity I/O runtime
- [ ] 9.2 Golden-path bot framework (navmesh, waypoint scripts, mechanic actions)
- [ ] 9.3 Mechanic module skeletons with tests: `status_bars`, `vehicle`, `heightfield`, `economy`, `squad`
- [ ] 9.4 Personalisation pack loader (local file / URL), defaults
- [ ] 9.5 Create changes `add-level-01-baublegum-city`, `add-level-02-sky-sled`, `add-level-03-operation-tinselium` from the concept docs
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
