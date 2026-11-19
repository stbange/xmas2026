# Spec Delta

## Purpose
The movement, combat basics, resources, checkpoints, saving, collectibles, difficulty and accessibility that work the same way in every level.

## ADDED Requirements

### Requirement: Shared verbs
In every level the player SHALL be able to move, look, jump, crouch, sprint, fire, alt-fire, melee, use, switch weapons, open the map and pause, with the same default bindings.

#### Scenario: Same controls everywhere
- **WHEN** the player enters any level
- **THEN** the default bindings for the shared verbs are unchanged from the hall

### Requirement: Base kit
The player SHALL always carry the Candy Cane (melee), the Snowball Blaster (infinite-ammo sidearm with charged shot) and the NOEL-Pad (map, objectives, hints), unless a level explicitly replaces the player's body (e.g. a mech or ship).

#### Scenario: Base kit in a new level
- **WHEN** the player starts any on-foot level
- **THEN** the Candy Cane and Snowball Blaster are in the weapon slots

### Requirement: Loaner equipment stays in its level
Weapons and items found in a level SHALL be available only in that level; leaving the level SHALL remove them.

#### Scenario: Leaving with a loaner
- **WHEN** the player leaves level 1 carrying the Gift-Shrinker
- **THEN** the Gift-Shrinker is not in the inventory in the hall

### Requirement: Health and armour
The player SHALL have health (100, up to 200 with overcharge that drains back to 100) and armour (0–100) that absorbs half of incoming damage.

#### Scenario: Armour absorbs damage
- **WHEN** the player has 100 health and 50 armour and takes 20 damage
- **THEN** health becomes 90 and armour becomes 40

### Requirement: Level resource bars are limited
A level SHALL add at most two extra resource bars (e.g. heat, mana, energy, oxygen, stamina, credits).

#### Scenario: Level validation
- **WHEN** a level declares three extra resource bars
- **THEN** the level validation fails

### Requirement: Checkpoints and death
Each level SHALL have checkpoints at least every 5 minutes of expected play; on death the player SHALL restart at the last checkpoint with the state recorded there.

#### Scenario: Death after a checkpoint
- **WHEN** the player dies after reaching checkpoint 3
- **THEN** the player respawns at checkpoint 3 with the health and ammo recorded there

### Requirement: Automatic saving
Progress (completed levels, collectibles, best times, alignment, trophies, unlocked portals, settings) SHALL be saved automatically and SHALL survive restarts of the game and of the browser.

#### Scenario: Browser restart
- **WHEN** the player completes level 2 in the web build and reloads the page
- **THEN** level 2 is shown as completed and its bauble is on the tree

### Requirement: Collectibles per level
Each level SHALL contain exactly one Memory Bauble (required to finish), three Gingerbread Floppies (secrets) and one Golden Joystick challenge.

#### Scenario: Results screen
- **WHEN** the player finishes a level having found 2 floppies
- **THEN** the results screen shows the bauble, "Floppies 2/3" and the challenge status

### Requirement: Difficulty levels
The game SHALL offer the difficulties Pizza, Cola (default), Coffee at 4 AM, and Nightmare LAN (unlocked after finishing level 24), which scale damage taken to 50 %, 100 %, 150 % and 200 %.

#### Scenario: Pizza damage
- **WHEN** the player on Pizza takes a hit worth 40 damage with no armour
- **THEN** the player loses 20 health

### Requirement: Co-op revive
In co-op a downed player SHALL be revivable by a teammate within 20 seconds; otherwise they SHALL respawn at the last checkpoint after 10 seconds.

#### Scenario: Revive in time
- **WHEN** a teammate holds Use on a downed player for the revive duration within 20 seconds
- **THEN** the downed player gets back up with 30 health

### Requirement: Accessibility options
The game SHALL provide full rebinding, hold/toggle options, subtitles for all dialogue and important sounds, colour-blind safe markers, FOV 60–110, head-bob and camera-shake toggles, flash/strobe reduction and a gore setting (Off, Confetti, Classic).

#### Scenario: Strobe reduction
- **WHEN** flash and strobe reduction is on and a level triggers a strobing light
- **THEN** the light changes at most three times per second
