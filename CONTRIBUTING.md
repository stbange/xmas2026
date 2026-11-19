# Contributing to Advent of LAN

Thanks for helping! A few rules keep the project healthy and legally clean.

## Licences
- **Code** you contribute (C, shaders, scripts, build files, tests, tools) is
  licensed under the **MIT License** (`LICENSE`).
- **Art and content** you contribute (textures, sprites, voxels, models, maps,
  audio, music, story text) is licensed under **CC BY 4.0** (`LICENSE-ART`).
- Add a row for every asset to `assets/ATTRIBUTION.md` (author, licence,
  source, changes). Third-party assets must be CC0 or CC BY 4.0.
- **Never** add assets, sounds, music, names or level layouts from the games
  that inspired the levels. See `docs/concept/10-legal-and-homage.md`.
- Never commit the private personalisation pack (`content/personal/`).

## Workflow
1. Behaviour changes start as an OpenSpec change in `openspec/changes/`
   (proposal, design, specs, tasks). Run `npx @fission-ai/openspec validate --strict`.
2. Every spec scenario needs a test tagged with
   `@scenario <capability>: <Scenario name>` (checked by `tools/ci/spec_trace.py`).
   Scenarios not implemented yet are listed in `openspec/trace-pending.txt`.
3. Development happens on `main`; every push must keep CI green.
   `gh-pages` is written only by CI.

## Build & test
```bash
cmake --preset linux-debug && cmake --build --preset linux-debug && ctest --preset linux-debug
```
Coverage gate (≥ 95 % lines, ≥ 90 % branches):
```bash
cmake --preset linux-coverage && cmake --build --preset linux-coverage && ctest --preset linux-coverage
python3 tools/ci/coverage_gate.py --build build/linux-coverage
```
Web build (needs the Emscripten SDK in `$EMSDK`):
```bash
cmake --preset web-playtest && cmake --build --preset web-playtest
```

## Code style
- C17, `clang-format` (see `.clang-format`), warnings are errors.
- Game logic lives in pure, testable modules (`src/core`, `src/sim`, …);
  only `src/platform/entry/` and the raw GL backend may stay untested.
- No hidden globals: time, randomness, files and the network come in through
  interfaces so tests can replace them.
