# Proposal

## Why

Portal 1 opens on 1 December 2026, eleven days from now. The hall is
playable, but every portal still leads to the shared test course. Day 1
needs its real level, "Baublegum City", and the level framework that all 24
levels build on: level definitions, scripted story beats, interactive world
objects and a bot that proves each level can be finished.

## What Changes

- **Level framework.** Add a level definition (manifest, geometry, entities,
  lights, props, script, spawn, golden path) with manifest validation and a
  registry for days 1–24. Days without a level yet keep the test course.
- **Level wiring in the simulation.** Add named entities, relays, movers
  (doors, screens, bridges), usable objects (switches, toilets, vending
  machines, arcade cabinets, buttons), light groups, hurt volumes, camera
  shake and an event queue that scripts react to.
- **Level scripts.** Add a sandboxed Lua 5.4 host: no io/os/package/load,
  `math.random` from the simulation's RNG, an instruction budget per callback
  and a memory cap. Scripts react to events and act only through the `tb`
  API, so levels stay deterministic.
- **Enemies.** Add ground and flying enemies with sight, chase, hitscan or
  projectile attacks, teleporting, a boss mortar, shrinking and stomping.
- **Day 1, "Baublegum City".** Mall roof → stairwell → concourse (toilet,
  cola, arcade, cinema) → toy store (Shrink-O-Matic and vent) → atrium
  (earthquake, collapsed bridge, jetpack) → parking deck (Bargain-Bin
  Battlelord) → exit. New gear: Mighty Boot, Pump Shotgun, Pipe Baubles,
  Gift-Shrinker (trophy), Jetpack and Holo-Santa. Three floppies and the
  "Interior Decorator" Golden Joystick.
- **Golden-path bot** that plays a level from waypoints and actions (walk,
  jetpack, use, select, fight, checkpoints). It runs as a unit test and with
  `--level N --bot` (`?level=N&bot=1` on the playtest site).
- **Playtest switches.** `--level N` / `?level=N` start straight in a level.
  Release builds ignore them, like `--playtest` and `--date`.

## Capabilities

### New Capabilities
- `homage-levels`: the individual levels; this change adds day 1.

### Modified Capabilities
- `level-framework`: adds level wiring, level scripts and placeholder courses.

## Impact

- New code: `src/game/{level,script,bot}.c`, `src/game/levels/day01.c`,
  `src/sim/{world,ai}.c`; level script at `content/levels/01/level.lua`
  (CC BY 4.0), embedded at build time.
- New dependency in use: Lua 5.4.9 (MIT), already declared.
- The test course stays as the placeholder for days 2–24.
