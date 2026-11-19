# Spec Delta

## Purpose
Keeps the project legally clean and shareable: MIT code, CC BY 4.0 art and content, full attribution, original homage content, and careful handling of the friends' personal data.

## ADDED Requirements

### Requirement: Licences
All source code SHALL be released under the MIT License and all art and content (textures, sprites, voxels, models, maps, audio, music, story text) under CC BY 4.0; third-party components SHALL use licences compatible with these.

#### Scenario: Incompatible dependency
- **WHEN** a dependency with a copyleft or non-commercial licence is proposed for the game binary
- **THEN** it is rejected in review

### Requirement: Attribution for every asset
Every file under the assets folder SHALL have an attribution entry (author, licence, source, changes), and the game's credits SHALL show all attributions.

#### Scenario: Missing attribution
- **WHEN** a new texture is added without an attribution entry
- **THEN** the attribution check in CI fails

### Requirement: Original homage content
The game SHALL NOT contain assets, music, sound effects, voice lines, level layouts, logos or character names from the inspiring games; inspiring game titles SHALL appear only in documentation and an "inspired by" credits list with a non-affiliation disclaimer.

#### Scenario: Credits disclaimer
- **WHEN** the player opens the credits
- **THEN** the "inspired by" list and the non-affiliation disclaimer are shown

### Requirement: Personal data stays private
Personalisation content (friends' nicknames, photos, texts) SHALL NOT be part of the public repository or the public gh-pages site, and SHALL only be shared with members of a session whose host loaded it.

#### Scenario: Public build
- **WHEN** the gh-pages site is built by CI
- **THEN** it contains no personalisation pack files

### Requirement: No tracking
The game SHALL NOT collect analytics or send telemetry; playtest timing logs SHALL be local and opt-in.

#### Scenario: Default settings
- **WHEN** the game runs with default settings
- **THEN** it makes no network requests other than loading its own files and, when playing online, connecting to the relay
