# Spec Delta

## Purpose
The automated checks that keep the game correct and the playtest site working: unit-test coverage, automated game tests and continuous integration gates.

## ADDED Requirements

### Requirement: Unit-test coverage gate
CI SHALL fail when unit-test line coverage of the game's C code is below 95 % or branch coverage is below 90 %. Only the platform glue and raw graphics-API backend MAY be excluded, and the excluded code SHALL stay below 5 % of the game's C code.

#### Scenario: Coverage drops
- **WHEN** a commit lowers line coverage to 94.8 %
- **THEN** the coverage job fails and the build is not published

#### Scenario: Exclusion grows too large
- **WHEN** excluded files reach 5 % or more of the game's C lines
- **THEN** the coverage job fails

### Requirement: Scenarios are tested
Every scenario in the OpenSpec specs SHALL be referenced by at least one unit or game test.

#### Scenario: Untested scenario
- **WHEN** a spec scenario has no test referencing it
- **THEN** the spec-trace check reports it and fails

### Requirement: Headless game mode
The game SHALL run in a headless mode without window, GPU or audio, simulating faster than real time with deterministic results.

#### Scenario: Deterministic replay
- **WHEN** the same replay is run twice in headless mode
- **THEN** both runs end with the same state checksum

### Requirement: Every level is proven completable
For every level, CI SHALL run a golden-path bot and the level's replays and invariant checks on each push to main.

#### Scenario: Broken door
- **WHEN** a change makes a required door in level 3 impossible to open
- **THEN** the level 3 golden-path test fails

### Requirement: Rendering regression tests
CI SHALL render fixed camera shots of the hall and each finished level with a software renderer and compare them to approved images within a tolerance.

#### Scenario: Unintended visual change
- **WHEN** a shader change alters the hall's image beyond the tolerance
- **THEN** the render test fails and shows a diff image

### Requirement: Network tests
CI SHALL simulate multi-client sessions with latency, jitter and packet loss over a loopback transport and verify synchronisation and completion of a co-op path.

#### Scenario: Packet loss
- **WHEN** 4 clients play the co-op test path with 10 % packet loss
- **THEN** all clients finish in sync with the host

### Requirement: Web end-to-end tests
Before publishing, CI SHALL load the web build in a real browser, enter the hall, enter portal 1 in playtest mode, verify touch mode with a mobile emulation, and connect two browser contexts into one session.

#### Scenario: Web build broken
- **WHEN** the web build fails to show the hall within 30 seconds
- **THEN** the e2e job fails and gh-pages is not updated

### Requirement: Robust parsers
Network packets, save files, content packages and imported asset formats SHALL be fuzz-tested, and malformed input SHALL be rejected without crashing.

#### Scenario: Corrupt save
- **WHEN** the save file is truncated
- **THEN** the game reports the corrupt save, keeps a backup, and starts with defaults
