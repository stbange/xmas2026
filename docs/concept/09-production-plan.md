# 09 — Production Plan

## Reality check
Today is **19 November 2026**. Day 1 opens on **1 December**, the last door on
**24 December**. That is 12 days until the first door and 35 days until the
last. 24 levels × 20 minutes is about **8 hours of handcrafted content**, plus
an engine, multiplayer, four platforms and XR. That is a lot for that time.

The plan makes this possible by:
1. **Shipping on the calendar.** A level has to be done only by **its own
   day** (two days before for testing), not by 1 December. The web build on
   `gh-pages` updates daily, so the friends always play the latest version.
2. **Engine first, then a level kit.** The first 12 days go into the engine,
   the hall and a reusable **level kit** (mechanic modules, enemy base
   behaviours, standard props). After that, a level is mostly content and
   wiring.
3. **Reuse mechanics across levels** (see the module table in
   [core gameplay](03-core-gameplay.md#mechanic-modules)).
4. **Cut scope in a defined order** (see below), never cut quality gates.
5. **Parallel work with AI-assisted development** and, if possible, a few
   helping friends (art, level layout, playtesting).

## Milestones
| # | Dates | Goal | Exit criteria |
|---|---|---|---|
| **M0 Bootstrap** | 19–22 Nov | repo, CMake, CI, OpenSpec, Unity, coverage gate, empty SDL3 window on Linux/Windows/Web, `gh-pages` deploy | CI green on all jobs, web build visible on gh-pages |
| **M1 Engine core** | 22–26 Nov | renderer (G-buffer, lights, shadows, palette), map compiler, player controller, collision, sprites, voxels, audio | the hall greybox walkable at 60 fps on web desktop and phone |
| **M2 Hall & calendar** | 26–29 Nov | the hall with tree, doors, portals, calendar unlock, save, NOEL, touch controls, gamepads, split screen | friends can walk the hall on 29 Nov; doors show countdowns |
| **M3 Level kit + days 1–3** | 26–30 Nov | weapons, enemies, AI, checkpoints, scripting, bots for game tests; levels 1–3 | levels 1–3 meet the definition of done |
| **M4 Online** | 30 Nov–6 Dec | relay, rooms, snapshots, prediction; Snowball Arena online | 4 players online across web/desktop |
| **M5 WebXR** | 6–10 Dec | XR session, stereo rendering, controllers, comfort | hall and day 1 playable in a Quest browser |
| **Rolling** | 1–24 Dec | level *N* done by day *N − 2* | per-level definition of done |
| **M6 Finale** | by 22 Dec | day 24, endings, epilogue, credits, Remix Mode | full playthrough by a bot and by a human |

### Rolling level schedule (done by)
| Level | Due | Level | Due | Level | Due |
|---|---|---|---|---|---|
| 1 | 29 Nov | 9 | 7 Dec | 17 | 15 Dec |
| 2 | 30 Nov | 10 | 8 Dec | 18 | 16 Dec |
| 3 | 1 Dec | 11 | 9 Dec | 19 | 17 Dec |
| 4 | 2 Dec | 12 | 10 Dec | 20 | 18 Dec |
| 5 | 3 Dec | 13 | 11 Dec | 21 | 19 Dec |
| 6 | 4 Dec | 14 | 12 Dec | 22 | 20 Dec |
| 7 | 5 Dec | 15 | 13 Dec | 23 | 21 Dec |
| 8 | 6 Dec | 16 | 14 Dec | 24 | 22 Dec |

That's one level per day while the friends are playing. The schedule is
front-loaded with the levels that build the most reusable modules (1 = FPS
core; 2 = flight + terrain; 3 = economy + squads).

## Scope cuts (in this order, if late)
1. Bonus versus variants (keep the three headline versus modes).
2. Remix Mode and Nightmare LAN (can come after Christmas).
3. WebRTC peer-to-peer (keep the relay).
4. Golden Joystick challenges.
5. Per-level shorten to ~12–15 minutes by dropping act 3's second set piece.
6. Third-person mode (day 4) → first person only.
7. WebXR → hall + selected "Comfortable" levels only.
Never cut: the calendar, the bauble/story, co-op, the tests and coverage gate,
touch controls.

## Risks
| Risk | Impact | Mitigation |
|---|---|---|
| Not enough time for 24 handcrafted levels | high | calendar-driven deadlines, level kit, reuse, scope cuts, help from friends |
| WebGL 2 performance on phones | medium | low internal resolution, forward fallback, dynamic resolution, early phone tests in M1 |
| WebXR browser differences | medium | target the Quest browser first; feature-detect; XR is optional |
| Network relay hosting | medium | tiny stateless relay; desktop LAN mode as fallback; local split screen always works |
| 95 % coverage slows development | medium | design for testability from day one (pure sim); generate test skeletons with the module |
| IP concerns about homages | medium | strict homage guidelines, original assets and names, non-commercial, inspirations credited (see [legal](10-legal-and-homage.md)) |
| Motion sickness in flight/6DOF levels | low | comfort options, cockpit frames, ratings |

## Working agreement
- `main` is the only development branch (as requested); every push must be
  green. Short-lived local branches are fine but are merged to `main` quickly.
- `gh-pages` is written only by CI (plus the initial placeholder).
- One OpenSpec change per level (`add-level-NN-<slug>`), plus changes for
  engine features.
