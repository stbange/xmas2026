# Spec Delta

## ADDED Requirements

### Requirement: Level wiring
Levels SHALL be able to name entities and connect them: relay volumes fire their targets when a player enters; movers (doors, screens, bridges) slide when fired; usable objects (switches, toilets, vending machines, arcade cabinets, buttons) react to the Use button within 2 m in plain sight; light groups switch on and off.

#### Scenario: Relay trigger
- **WHEN** a player walks into a relay volume
- **THEN** the relay reports its name once on entry and fires its target, and a one-shot relay never fires again

#### Scenario: Door opened by a switch
- **WHEN** the player uses a switch whose target is a closed door
- **THEN** the switch toggles its light group and the door slides open, so shots and players pass where it stood

#### Scenario: Usable world objects
- **WHEN** the player uses a vending machine
- **THEN** they gain 5 health, the use is reported to the level script, and the machine needs half a second before it can be used again

### Requirement: Level scripts
Every level MAY have a Lua 5.4 script that reacts to level events and acts on the level only through the engine's script API. Scripts SHALL run in a sandbox without file, OS, package or code-loading access, with random numbers from the deterministic simulation, an instruction budget per callback and a memory cap.

#### Scenario: Sandboxed script
- **WHEN** a level script looks for `io`, `os`, `package`, `debug`, `load`, `loadfile`, `dofile`, `require` or `collectgarbage`
- **THEN** none of them exist, and `math.random` draws from the simulation's seeded generator

#### Scenario: Script error
- **WHEN** a level script raises an error
- **THEN** the error is logged with its location, the script stays silent from then on, and the level remains playable

#### Scenario: Runaway script
- **WHEN** a script callback loops forever or allocates without bound
- **THEN** it is stopped by the instruction budget or the memory cap

### Requirement: Placeholder levels
A portal whose level is not built yet SHALL lead to the shared test course with a bauble, three floppies and an exit, so every open portal can be completed.

#### Scenario: Placeholder course
- **WHEN** a player enters a portal whose level has not been delivered
- **THEN** the test course loads with a Memory Bauble, three floppies and an exit, and the briefing says it is a test course
