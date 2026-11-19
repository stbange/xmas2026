# Spec Delta

## Purpose
The contract every one of the 24 homage levels fulfils, so levels can be built one per day on a shared foundation and verified automatically.

## ADDED Requirements

### Requirement: Level manifest
Every level SHALL declare a manifest with: day number (1–24), original in-game title, inspiration (for credits only), 2–3 signature mechanics, loaner weapons/items, the trophy, extra resource bars, input schema, XR comfort rating, palette, music track and expected play time.

#### Scenario: Missing manifest field
- **WHEN** a level is cooked without a trophy in its manifest
- **THEN** the content build fails with an error naming the level and field

### Requirement: Homage mechanics
Every level SHALL introduce two or three gameplay mechanics, weapons or items inspired by its source game, on top of the shared player core.

#### Scenario: Mechanic count
- **WHEN** a level's manifest lists one or more than three signature mechanics
- **THEN** level validation fails

### Requirement: Twenty-minute structure
Every level SHALL follow the structure briefing → act 1 (learn) → act 2 (combine) → act 3 (twist) → finale (Glitch Lieutenant and bauble) → exit, with a target median play time of 15–25 minutes on Cola difficulty.

#### Scenario: Bot lower bound
- **WHEN** the golden-path bot completes a level
- **THEN** its simulated time is between 6 and 18 minutes

### Requirement: Completion
A level SHALL be completed when the player (or any co-op player) collects the Memory Bauble and reaches the exit; completion SHALL be recorded once per player.

#### Scenario: Co-op completion
- **WHEN** player 2 collects the bauble and both players reach the exit
- **THEN** the level is recorded as completed in both players' saves

### Requirement: Input schema
Every level-specific action SHALL be declared in the level's input schema with bindings for keyboard/mouse, gamepad, touch and XR controllers.

#### Scenario: Touch button for a level action
- **WHEN** a level declares the action `jetpack`
- **THEN** a jetpack button appears in touch mode while the action is available

### Requirement: Reusable mechanic modules
Mechanics used by more than one level (flight, vehicles, terrain deformation, voxel destruction, squads, economy, tactical time, status bars, spells, rounds, procedural dungeons, loot, alignment) SHALL be implemented once as engine modules and configured per level.

#### Scenario: Shared flight
- **WHEN** level 20's cyberspace and level 11 both use six-degrees-of-freedom flight
- **THEN** both use the same flight module with level-specific parameters

### Requirement: Briefing in homage style
Every level SHALL start with a skippable briefing of 20–60 seconds presented in a style reminiscent of its source game, followed by a one-line NOEL summary.

#### Scenario: Skipping a seen briefing
- **WHEN** the player enters a level whose briefing they have seen before and presses Skip
- **THEN** the level starts immediately

### Requirement: Per-level content packages
Each level SHALL be packaged as its own content package that can be delivered separately from the engine and the hall.

#### Scenario: Daily delivery
- **WHEN** the web build is deployed without the package for level 12 and portal 12 is open
- **THEN** the portal shows "arriving soon" instead of failing, and the package is loaded once it is published

### Requirement: Automated test hooks
Every level SHALL provide a golden-path bot script, at least one recorded replay, and level invariants (key/lock order, reachability of bauble and floppies) used by the automated game tests.

#### Scenario: Unreachable secret
- **WHEN** a floppy is placed where the navigation data cannot reach it
- **THEN** the level invariant check fails
