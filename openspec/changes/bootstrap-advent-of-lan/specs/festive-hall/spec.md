# Spec Delta

## Purpose
The hub level that the player walks through in first person: it holds the 24 portals, the Christmas tree that shows progress, and the social and story spaces between levels.

## ADDED Requirements

### Requirement: Hall layout
The hall SHALL contain 24 numbered advent doors with portals (1–12 on the ground floor, 13–24 on a balcony), a central Christmas tree, NOEL's desk, a trophy wall, a jukebox, a cocoa bar, a LAN corner with statistics, and a Snowball Arena.

#### Scenario: Finding a door
- **WHEN** the player looks for door 17
- **THEN** it is on the balcony, labelled "17", and reachable by walking

### Requirement: Portal preview
Each open portal SHALL show a live low-resolution preview of its level's entry scene; closed portals SHALL show a frosted, closed door.

#### Scenario: Newly opened portal
- **WHEN** a portal opened today and the player has not entered it yet
- **THEN** it shows its live preview, a ribbon and a "NEW!" tag

### Requirement: Entering a level
Walking into an open portal SHALL start that level's briefing and then the level; finishing or leaving a level SHALL return the player to the hall in front of that portal.

#### Scenario: Return after finishing
- **WHEN** the player finishes level 5
- **THEN** the player stands in front of portal 5 in the hall and the bauble-hanging scene plays

### Requirement: Tree shows progress
Each collected Memory Bauble SHALL appear on the tree and light one ring of the tree's lights.

#### Scenario: Three baubles
- **WHEN** the player has collected the baubles of levels 1, 2 and 4
- **THEN** three baubles hang on the tree and three rings of lights are lit

### Requirement: Hall reflects the story act
The hall's mood SHALL change with the calendar and the player's progress through the acts BOOT (days 1–8), DEFRAG (9–16) and SHUTDOWN (17–24), and SHALL be fully restored after the finale.

#### Scenario: Shutdown act
- **WHEN** the effective date is 2026-12-18 and the player has finished level 17
- **THEN** the hall uses the SHUTDOWN emergency lighting

#### Scenario: Restored hall
- **WHEN** the player has finished level 24
- **THEN** the hall is fully lit and decorated and the tree has its top ornament

### Requirement: Trophy wall
Finishing a level SHALL place that level's trophy on the trophy wall, from where it can be taken into the Snowball Arena, Remix Mode and the finale.

#### Scenario: Taking a trophy
- **WHEN** the player has finished level 1 and uses its trophy on the wall
- **THEN** the trophy weapon is available in the Snowball Arena

### Requirement: Snowball Arena always available
The Snowball Arena SHALL be playable from the first start of the game, before any portal is open, alone with bots or with other players.

#### Scenario: November arena
- **WHEN** two friends start a session on 2026-11-28
- **THEN** they can play a snowball match in the arena

### Requirement: Hall runs on phones
The hall SHALL keep at least 30 frames per second on the reference mid-range phone in the web build.

#### Scenario: Phone performance
- **WHEN** the hall is shown on the reference phone with automatic quality
- **THEN** the measured frame rate stays at or above 30 fps for a 60-second walk-through
