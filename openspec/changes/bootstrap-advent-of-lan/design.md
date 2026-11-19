# Design

## Context

Greenfield repository (only a licence file). The game concept is in
`docs/concept/`; the technical detail is in `docs/concept/05-technology.md`
to `08-quality-and-testing.md`. Hard constraints from the request: C,
SDL3, OpenGL; Windows, Linux, Web via Emscripten, WebXR; touch on mobile web;
split screen and network multiplayer (especially on the web); OpenSpec; ≥95 %
unit coverage; automated game tests; MIT code / CC BY 4.0 art; development on
`main`, playtest build on `gh-pages`. Day 1 must be playable on 2026-12-01.

## Goals / Non-Goals

**Goals:**
- One C17 code base for all targets, built with CMake presets.
- A pure, deterministic simulation that can be unit-tested, replayed,
  run headless by bots, and networked host-authoritatively.
- A renderer that achieves the retro look with modern lighting on WebGL 2 phones.
- A level kit that makes one ~20-minute level per day feasible.
- CI that blocks regressions and publishes the web build automatically.

**Non-Goals:**
- Dedicated servers, matchmaking with strangers, accounts.
- macOS/iOS/Android native apps, consoles, desktop OpenXR.
- A player-facing level editor.
- Voice chat.

## Decisions

1. **C17 + SDL3 + GL 3.3/GLES 3.0 common subset.** One renderer for desktop
   and WebGL 2. *Alternatives:* bgfx/sokol (another abstraction layer; GL is
   explicitly requested), WebGPU (not available everywhere, not requested).

2. **Layered architecture with a pure core** (`core`, `sim`, `net` protocol,
   audio mixer, renderer CPU side) and a thin `platform` layer. This is what
   makes the 95 % coverage target realistic: everything that matters runs
   without a window. *Alternative:* covering SDL/GL calls via mocking —
   brittle and low value.

3. **Fixed 60 Hz deterministic simulation, all randomness seeded.** Enables
   replays, golden-path bots, network prediction and the daily dungeon seed.
   Determinism is required per build/platform (replays are verified on
   Linux CI), not bit-identical across platforms, because online play is
   host-authoritative.

4. **Deferred lighting at low internal resolution, forward fallback.** At
   480×270 a G-buffer is tiny, so dozens of dynamic lights are cheap even on
   phones; palette quantisation runs as a LUT post-pass. *Alternative:*
   forward+ only — more shader permutations, harder on WebGL 2.

5. **Levels authored in TrenchBroom (`.map`), voxels in `.vox`, models in
   glTF, compiled by our own `tbcook` into one `.pak` per level.** Proven
   retro workflow, free tools, and per-level packages enable daily delivery.

6. **Mechanics as C modules, choreography in Lua.** Reusable mechanics need
   coverage and performance; level scripts are short and covered by game tests.

7. **Networking: host-authoritative listen server; WebSocket relay first,
   WebRTC later, ENet for desktop LAN.** The relay works from GitHub Pages,
   through NAT and across all platforms with the least code. *Alternative:*
   WebRTC-only P2P — better latency but needs signalling and TURN anyway and
   more JS glue; deferred to phase 2.

8. **WebXR via a small JS library called from C each XR frame.** Emscripten
   has no built-in WebXR binding; a thin wrapper keeps the C side
   platform-neutral (view/projection matrices + controller poses in, one
   framebuffer out).

9. **Testing stack:** Unity + CTest for unit tests, gcovr for coverage gates,
   a headless game binary for bots/replays/invariants, Mesa llvmpipe for
   render goldens, Playwright for web e2e, libFuzzer nightly.

10. **Branching:** all development on `main` (as requested) with CI gates;
    `gh-pages` is an orphan branch written only by the deploy job.

11. **Date logic** lives in `game/calendar` as a pure function
    `open_portals(local_datetime, server_time?, persisted_unlocks, flags)`, so
    every scenario of the `advent-calendar` spec is a table-driven unit test.

## Risks / Trade-offs

- **Schedule:** 24 handcrafted levels in 35 days → calendar-driven deadlines,
  level kit, defined scope-cut order (see `docs/concept/09-production-plan.md`).
- **Coverage vs speed:** 95 % takes discipline → test-first for pure modules;
  the platform layer is kept thin and measured.
- **WebGL 2 on low-end phones:** deferred may be slow → automatic forward
  fallback and dynamic resolution.
- **Relay hosting:** an external service is needed → tiny stateless relay,
  LAN mode and split screen still work without it.
- **IP:** homages near famous franchises → strict homage rules in
  `docs/concept/10-legal-and-homage.md`, review checklist per level.
- **Determinism pitfalls** (floating point, iteration order) → replay tests
  on every push catch divergences early.
