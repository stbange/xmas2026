# Spec Delta

## Purpose
The frame story that connects the 24 levels — recovering the Memory Baubles from KRAMPUS.EXE — including the Nice/Naughty alignment and the two endings.

## ADDED Requirements

### Requirement: Memory vignette per bauble
Collecting a level's Memory Bauble SHALL play that level's memory vignette (text plus optional image) and SHALL add the memory to the memory album in the hall.

#### Scenario: Re-watching a memory
- **WHEN** the player opens the memory album at NOEL's desk after finishing level 3
- **THEN** the memory of level 3 can be replayed

### Requirement: Story acts
The story SHALL be organised into BOOT (levels 1–8), DEFRAG (9–16) and SHUTDOWN (17–24), with the KRAMPUS.EXE origin reveal in level 16, NOEL's capture and rescue in level 20, and the final confrontation in level 24.

#### Scenario: Playing out of order
- **WHEN** the player finishes level 16 before level 13
- **THEN** the level 16 reveal still plays and the story remains understandable (no reference requires level 13's events)

### Requirement: Nice/Naughty alignment
The game SHALL track a Nice/Naughty alignment per player, with small contributions from levels 1–16 and full weight from level 17 onwards, persisted in the save.

#### Scenario: Sparing a surrendering enemy
- **WHEN** in level 17 the player spares an enemy who surrenders
- **THEN** the alignment moves towards Nice

### Requirement: Two endings with a second chance
At the end of level 24 the player SHALL choose between Invite and Delete; a strongly Naughty alignment SHALL offer only Delete at first. After the Delete ending a "Restore from Recycle Bin?" dialog SHALL allow switching to the Invite ending.

#### Scenario: Naughty player restores
- **WHEN** a strongly Naughty player chooses Delete and then answers Yes to "Restore KRAMPUS.EXE from Recycle Bin?"
- **THEN** the Invite ending plays and KRAMPUS.EXE becomes the tree's top ornament

### Requirement: Personalisation pack
The game SHALL accept an optional personalisation pack (crew nicknames, memory texts and images, spray tags) loaded from a local file or a URL, and SHALL use its content in place of the defaults; without it, default content SHALL be used.

#### Scenario: Custom memory
- **WHEN** a pack with `memories/07.txt` is loaded and the player collects bauble 7
- **THEN** the vignette shows the text from the pack

#### Scenario: Bots named after the crew
- **WHEN** a pack with five crew nicknames is loaded and level 7 starts
- **THEN** the enemy bots use those nicknames
