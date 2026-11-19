# Proposal

## Why

A group of friends who grew up with 1990s PC games should get a playable,
shareable advent calendar game for December 2026: a festive first-person
hall with 24 portals, one opening per day from 1 to 24 December, each leading
to a ~20-minute homage level. Day 1 is twelve days away, so the foundation
(engine, hall, calendar, controls, multiplayer, platforms, quality gates)
must be specified now, and levels must be addable one per day on top of it.
The full game concept is in `docs/concept/`.

## What Changes

- Establish the game's behaviour contract for the hub hall, the date-based
  portal unlock, the shared player core, the level framework that all 24
  levels plug into, and the meta story with its two endings.
- Specify the retro rendering style (low internal resolution, palettes,
  sprites/voxels/models, dynamic lights and shadows).
- Specify input on all devices, including automatic touch controls on mobile
  web and WebXR controllers.
- Specify split-screen and online multiplayer, including cross-play with the
  web version.
- Specify the supported platforms and the `gh-pages` playtest build.
- Specify the quality gates (≥95 % unit coverage, automated game tests) and
  the licensing rules (MIT code, CC BY 4.0 art, original homage content).
- Individual levels are **not** part of this change; each level gets its own
  change (`add-level-NN-<slug>`) that adds requirements to `homage-levels`.

## Capabilities

### New Capabilities
- `advent-calendar`: which portals are open on which date, persistence of unlocks, playtest overrides, clock sanity checks.
- `festive-hall`: the hub level — tree, doors, portals, NOEL, trophy wall, arena, and how the hall changes with progress.
- `player-core`: shared movement, base kit, health/armour, checkpoints, saving, collectibles, difficulty, accessibility.
- `level-framework`: what every level must provide (≈20-minute structure, bauble, secrets, loaner weapons, trophy, input schema, briefing, test hooks) and how levels are delivered.
- `meta-story`: the frame story, the Nice/Naughty alignment and the endings.
- `retro-rendering`: low-resolution, palette-based look with dynamic lighting and shadows, sprites, voxels and 3D models; performance targets.
- `input-controls`: keyboard/mouse, gamepad, touch (auto-switching) and XR controllers, rebinding, aim assist.
- `multiplayer`: local split screen, online sessions via room codes, co-op progress, versus modes, netcode behaviour.
- `platform-targets`: Windows, Linux, Web and WebXR builds, saves per platform, the `gh-pages` playtest deployment.
- `quality-gates`: unit-test coverage thresholds, automated game tests, CI gates.
- `content-licensing`: licences, attribution, homage rules, personal data handling.

### Modified Capabilities
- None (greenfield project).

## Impact

- New repository structure: `src/`, `tests/`, `tools/`, `assets/`, `web/`,
  `docs/`, `openspec/`, CI workflows in `.github/workflows/`.
- New dependencies (all permissive): SDL3, cglm, cgltf, stb, Lua 5.4,
  cJSON, ENet, Unity; toolchain: CMake, Emscripten, gcovr, Playwright.
- New external service: a small WebSocket relay for online play (hosted
  outside GitHub Pages).
- The `gh-pages` branch becomes the public playtest site, updated by CI.
