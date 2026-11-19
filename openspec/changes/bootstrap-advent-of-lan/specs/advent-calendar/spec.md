# Spec Delta

## Purpose
Decides which of the 24 portals are open at any moment, so that one new level becomes available each day from 1 to 24 December 2026 and stays available afterwards.

## ADDED Requirements

### Requirement: Daily portal unlock
The game SHALL open portal N (1–24) when the effective date is on or after 2026-12-N 00:00 in the player's local time zone. Before 2026-12-01 no portal SHALL be open.

#### Scenario: Portal opens at local midnight
- **WHEN** the effective local time changes from 2026-12-06 23:59:59 to 2026-12-07 00:00:00
- **THEN** portal 7 becomes open and portals 8–24 remain closed

#### Scenario: Before December
- **WHEN** the effective local date is 2026-11-30
- **THEN** all 24 portals are closed and each shows its countdown

#### Scenario: After Christmas Eve
- **WHEN** the effective local date is 2026-12-25 or any later date
- **THEN** all 24 portals are open

### Requirement: Unlocks are permanent
Once a portal has been open on a device, it SHALL stay open on that device, even if the clock is later set back.

#### Scenario: Clock set back
- **WHEN** portal 10 was opened on 2026-12-10 and the device clock is later set to 2026-12-05
- **THEN** portal 10 is still open

### Requirement: Missed days can be played later
Open portals SHALL be playable in any order, at any time.

#### Scenario: Catching up
- **WHEN** a player starts the game for the first time on 2026-12-15
- **THEN** portals 1–15 are open and each can be entered

### Requirement: Closed portal feedback
A closed portal SHALL show its day number and a countdown to its opening, and SHALL NOT let the player enter.

#### Scenario: Bumping into a closed door
- **WHEN** the player uses a closed portal 20 on 2026-12-18 12:00 local time
- **THEN** the portal stays closed and shows "opens in 1 d 12 h"

### Requirement: Session date in online play
In an online session the host's effective date SHALL decide which portals are open for all members of the session.

#### Scenario: Client is ahead of the host
- **WHEN** the host's date is 2026-12-03 and a client's date is 2026-12-04
- **THEN** in that session portal 4 is closed for everyone

### Requirement: Clock sanity check
When the relay server time is available and the local clock is more than 36 hours ahead of it, the game SHALL use the server time as the effective date and SHALL tell the player in a humorous message.

#### Scenario: Time traveller
- **WHEN** the server time is 2026-12-02 10:00 UTC and the local clock says 2026-12-10
- **THEN** the effective date is derived from the server time and NOEL shows the time-travel message

### Requirement: Playtest override
Playtest builds SHALL accept a playtest switch (`?playtest=1` on the web, `--playtest` on desktop) that opens all portals, and an explicit date (`?date=YYYY-MM-DD`, `--date`). Release builds SHALL ignore both parameters.

#### Scenario: Playtesting all levels early
- **WHEN** the playtest build on the gh-pages site is opened with `?playtest=1` on 2026-11-25
- **THEN** all 24 portals are open and a debug overlay is visible

#### Scenario: Release build ignores override
- **WHEN** a release build is started with `--date 2026-12-24` on 2026-12-02
- **THEN** only portals 1 and 2 are open

#### Scenario: Release build ignores playtest switch
- **WHEN** the release build on the gh-pages site is opened with `?playtest=1` on 2026-12-02
- **THEN** only portals 1 and 2 are open and no debug overlay is shown
