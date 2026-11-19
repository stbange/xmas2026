# 08 — Quality & Testing

## Goals
1. **≥ 95 % line coverage** (and ≥ 90 % branch coverage) of the game's C code
   by unit tests, enforced in CI.
2. **Every level is proven completable** by automated game tests on every
   commit to `main`.
3. **The web build on `gh-pages` always works**: smoke-tested in a real
   browser before it is published.

## Spec-driven development with OpenSpec
- Every feature starts as an **OpenSpec change** in `openspec/changes/<name>/`
  with `proposal.md`, `design.md`, delta `specs/` and `tasks.md`.
- Requirements use `### Requirement:` with `#### Scenario:` (WHEN/THEN).
  **Every scenario maps to at least one test**: unit or game test names
  reference the requirement, e.g.
  `test_calendar__portal_opens_at_local_midnight` for the scenario
  "Portal opens at local midnight".
- When a change is implemented and archived (`openspec archive`), its deltas
  merge into `openspec/specs/`, which describes the current behaviour.
- CI runs `openspec validate --all --strict`.

## Unit tests
- **Framework:** Unity (ThrowTheSwitch), plain C; one test file per module
  (`tests/unit/test_<module>.c`), run with CTest.
- **What is unit-tested:** everything in `core/`, `sim/`, `game/`, `net/`
  (protocol, snapshots, prediction), `audio/` (mixer), the CPU side of
  `render/` (culling, sorting, voxel meshing, atlas packing, LUTs), and the
  content compiler `tools/tbcook/`.
- **Testability rules:** no hidden globals; time, randomness, files and the
  network come in through interfaces (function tables) with fakes for tests;
  the sim runs without a window or GPU.
- **Coverage:** built with `--coverage` (GCC) and reported with `gcovr`.
  The CI job fails below **95 % lines / 90 % branches**.
- **What is excluded from the coverage metric** (and must stay thin): the
  files in `src/platform/` (SDL3/GL/Emscripten/WebXR glue) and the GL
  backend's raw API calls in `src/render/gl/`. The exclusion list is
  explicit in `gcovr.cfg`, reviewed in PRs, and the excluded code must stay
  **under 5 % of all game C code** (CI checks the ratio). Those parts are
  covered by the game, render and web tests instead.
- **Sanitisers:** a CI job runs all unit tests with AddressSanitizer and
  UndefinedBehaviorSanitizer.
- **Static analysis:** `clang-tidy` and `cppcheck` with warnings as errors;
  `-Wall -Wextra -Werror` (plus `/W4 /WX` on MSVC).

## Automated game tests
The game has a **headless mode** (`--headless`): null renderer, null audio,
the sim running as fast as possible.

| Test type | What it does | When |
|---|---|---|
| **Golden-path bots** | A test bot (navmesh + per-level waypoint script that uses the level's mechanics) plays each level from start to bauble and asserts completion within a time budget (sim time). | every push |
| **Replay tests** | Recorded inputs (`.tbdemo`) are replayed; the final state checksum must match. Detects non-determinism and unintended gameplay changes. Re-recorded consciously when a level changes. | every push |
| **Level invariants** | Static checks on cooked levels: every key/lock reachable in order, the bauble and all 3 floppies reachable, no entity outside the world, all scripted targets exist, input schema complete. | every push |
| **Chaos monkey** | Random inputs for 10 sim-minutes per level: no crash, no NaN, no entity leak, no stuck "can't progress" state (checked by the invariant oracle). | nightly |
| **Render goldens** | Fixed camera shots per level rendered offscreen with Mesa llvmpipe (EGL surfaceless) and compared with golden images using a perceptual tolerance. | every push |
| **Performance budget** | Headless sim tick time for the heaviest scenes (days 10, 23) must stay under budget; frame-time captures from the reference GPU are a manual check before each release. | every push (sim) |
| **Network tests** | Two to eight in-process clients on a loopback transport with simulated latency, jitter and loss (0–20 %); they must stay in sync and complete a co-op golden path; versus rounds resolve correctly. | every push |
| **Fuzzing** | libFuzzer on the packet parser, save-file loader, `.pak` reader and `.vox`/`.map` importers. | nightly |
| **Web e2e** | Playwright starts the web build in Chromium (and Firefox): loads, shows the hall, enters portal 1 in `?playtest=1`, completes a short scripted path, checks touch mode with a mobile device profile, joins two browser contexts into one room via a local relay. | before publishing to `gh-pages` |
| **WebXR smoke** | Playwright with the WebXR emulator / mocked `navigator.xr`: enter VR, render a few frames, exit. | before publishing |

### Playtime check (the "20 minutes")
- The golden-path bot gives the **lower bound** (should be 8–12 min of sim time).
- Human playtests are logged (opt-in, local file) with time per act; the
  target is a **median between 15 and 25 minutes** on *Cola*.

## Continuous integration (GitHub Actions)
```
on: push (main), pull_request
jobs:
  spec:      openspec validate --all --strict
  lint:      clang-format check, clang-tidy, cppcheck
  linux:     gcc + clang: build, unit tests, sanitisers
  coverage:  gcc --coverage, gcovr ≥ 95 % lines / 90 % branches, platform ratio ≤ 5 %
  windows:   MSVC build + unit tests
  content:   tbcook all levels, level invariants
  gametests: headless bots + replays + network loopback (Linux)
  render:    llvmpipe render goldens
  web:       emscripten build → Playwright e2e → upload artifact
  deploy:    (main only, all green) publish web build to gh-pages
nightly:     chaos monkey, fuzzers, long network soak
```

## The `gh-pages` playtest build
- On every green push to `main`, CI publishes the web build to the
  `gh-pages` branch (root), served at `https://stbange.github.io/xmas2026/`.
- `?playtest=1` opens all portals and shows a debug overlay (fps, build hash,
  sim tick).
- Each build shows its git hash in the BIOS-gate screen for bug reports.
- Optional daily content-pack publishing for the real December run (see
  [the hall](02-the-hall.md#content-drop-spoiler-protection-optional)).

## Definition of done (per level)
- [ ] OpenSpec change archived; all its scenarios have tests.
- [ ] Unit coverage ≥ 95 % still holds.
- [ ] Golden-path bot, replay, invariants and render goldens green.
- [ ] Co-op golden path green with 2 players over loopback.
- [ ] Touch-controls pass on a phone (manual) and XR comfort rating set.
- [ ] One human playtest within 15–25 minutes.
- [ ] All assets have attribution entries; no third-party IP.
