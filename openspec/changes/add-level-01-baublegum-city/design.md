# Design

## Context

Levels must be deterministic: replays, network play and the golden-path bot
all rely on identical inputs producing identical states. Level content has
to arrive one level per day, so per-level work has to be mostly data and a
short script.

## Goals / Non-Goals

**Goals:** a level contract that day 2–24 can follow without new
engine work for common things (doors, buttons, triggers, enemies, story
beats); a playable day 1; automatic proof that a level can be finished.

**Non-Goals:** the map compiler and content packages (task group 6),
navmesh-based bots, sprite and model art for enemies (boxes for now),
sound.

## Decisions

### Levels in code until the map compiler exists
A `tb_level_def` holds the manifest, a geometry builder, a populate function,
lights, props, the Lua script and the golden path. Day 1 builds its brushes
from a table. Once maps are compiled, the builder becomes a loader and the
rest of the contract stays the same.

### Simulation events, not callbacks into Lua from the simulation
The simulation queues events (relay entered, object used, named enemy killed,
pickup taken), up to 32 per tick. After each step the script host sends them
to `on_trigger`, `on_used`, `on_killed` and `on_pickup`. The simulation never
calls Lua, so it stays pure C and the script runs at a single point in the
tick. Script effects (spawning, firing targets, lights, shake, objective)
change the simulation through the `tb` API before the next step.

### Sandbox
Lua runs with only `base`, `table`, `string` and `math`. `load`, `dofile`,
`require`, `io`, `os`, `debug` and `collectgarbage` are removed. Only text
chunks are accepted. `math.random` uses the simulation's PCG32 and
`math.randomseed` is removed. Each callback gets a budget of 200 000 VM
instructions, enforced with a count hook, and the state has a 2 MiB
allocator cap. A script error is logged and silences the script; the level
stays playable.

### Movers rewrite brushes
A mover owns one axis-aligned box brush and rewrites its six planes as it
moves. Traces, shots and enemies see the door where it currently is. The
renderer skips mover brushes in the static mesh and draws them as dynamic
boxes.

### Enemy movement reuses player movement
Ground enemies move with the player movement code, steered by a synthetic
input. Flying enemies sweep their own box. This keeps stairs, slopes and
gravity consistent. The trade-off is that enemies collide with the player
hull rather than their own size, which is acceptable at this scale.

### The bot follows waypoints and fights what it sees
The bot works from a list of steps: go to, fly with the jetpack, use, wait,
select a weapon, clear the area near a point, and checkpoint. While on any
step it fights the nearest visible enemy. After dying it resumes at the last
checkpoint step, and it gives up after 8 deaths or 40 s on one step. A
navmesh is deferred.

## Risks / Trade-offs

- **Play time.** The bot finishes day 1 in about 1.5 simulated minutes; a
  human who explores is expected to take 8–12 minutes, below the 15–25 minute
  target. The "Bot lower bound" scenario stays pending, and the level grows
  as art and set pieces arrive.
- **Difficulty.** On Nightmare the bot sometimes loses to the boss; that is
  intended.
