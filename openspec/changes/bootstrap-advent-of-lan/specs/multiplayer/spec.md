# Spec Delta

## Purpose
Playing together: local split screen on one device and online sessions across Web, Windows and Linux, for co-op through the levels and for versus modes.

## ADDED Requirements

### Requirement: Local split screen
On desktop and on large-screen web, the game SHALL support up to 4 local players in split screen, with player 1 on keyboard/mouse or gamepad and the others on gamepads. Split screen SHALL be unavailable on phones.

#### Scenario: Second gamepad joins
- **WHEN** a second gamepad presses Start in the hall
- **THEN** the screen splits and player 2 spawns next to player 1

### Requirement: Online sessions with room codes
A player SHALL be able to host an online session and receive a room code and an invite link; other players SHALL join by entering the code or opening the link. Sessions SHALL hold up to 4 players in co-op and up to 8 in versus.

#### Scenario: Joining by link
- **WHEN** a friend opens the invite link of an existing room in a browser
- **THEN** the friend joins the host's session in the hall

#### Scenario: Room full
- **WHEN** a fifth player tries to join a co-op session
- **THEN** the join is refused with a "room full" message

### Requirement: Cross-play
Web, Windows and Linux builds of the same protocol version SHALL be able to play together; mismatched versions SHALL be refused with a clear message.

#### Scenario: Version mismatch
- **WHEN** a client with protocol version 3 tries to join a host with version 4
- **THEN** the client sees "Please update — the host uses a newer version"

### Requirement: Combined local and online
A session SHALL allow local split-screen players on a device to join an online session together.

#### Scenario: Two couches
- **WHEN** two players on one PC join an online session hosted by two players on another PC
- **THEN** all four play in the same session

### Requirement: Co-op progress
In co-op, finishing a level SHALL count for every player present at the end, and each collectible SHALL count for every player present when it was collected; each player SHALL keep their own save.

#### Scenario: Late joiner
- **WHEN** a player joins after the first floppy was collected and stays until the end
- **THEN** the level counts as completed for them, but the first floppy does not

### Requirement: Responsive movement under latency
Online clients SHALL see their own movement and firing respond immediately (prediction) and other players' movement smoothly (interpolation) at up to 150 ms round-trip time and 5 % packet loss.

#### Scenario: Lossy connection
- **WHEN** a client plays with 150 ms RTT and 5 % loss
- **THEN** the client's own movement shows no visible delay and stays in sync with the host

### Requirement: Fair hit registration
For hit-scan weapons the host SHALL compensate for the shooter's latency up to 200 ms.

#### Scenario: Shot at a moving target
- **WHEN** a client with 100 ms latency hits a moving target on their screen
- **THEN** the host registers the hit

### Requirement: Host leaves
If the host leaves, all clients SHALL be returned to their own hall with their progress saved.

#### Scenario: Host quits mid-level
- **WHEN** the host quits during level 9
- **THEN** each client returns to their hall and their collected items up to that point are saved

### Requirement: Versus modes
The game SHALL offer versus modes in the Snowball Arena and in the headline versus levels (rounds in level 7, capture the flag in level 9, deathmatch in level 16), with bots filling empty slots.

#### Scenario: Two humans in CTF
- **WHEN** two players start level 9 versus with 4-player teams
- **THEN** bots fill the six remaining slots

### Requirement: LAN mode
Desktop builds SHALL be able to find and join sessions on the local network without the relay.

#### Scenario: Offline LAN party
- **WHEN** two desktop PCs are on the same LAN without internet
- **THEN** one can host and the other can find and join the session
