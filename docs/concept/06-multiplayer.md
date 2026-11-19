# 06 — Multiplayer

## Modes
| Mode | Players | Where |
|---|---|---|
| **Co-op story** | 1–4 | every level and the hall |
| **Snowball Arena** | 2–8 | the hall's arena, from day 1 |
| **Versus (headline)** | 2–8 | day 7 (rounds), day 9 (CTF), day 16 (deathmatch) |
| **Versus (bonus)** | 2–8 | small variants of other levels (see level docs) |
| **Daily dungeon race** | 1–4 per team | day 12 with the daily seed |

Bots fill empty slots in versus modes (bots named after the crew).

## Local: split screen
- Up to **4 local players** on desktop and on the web with a large screen.
  On phones split screen is disabled.
- Layouts: 2 = vertical split (side by side) or horizontal (option); 3 = one
  big + two small; 4 = quadrants.
- Input: player 1 can use keyboard+mouse, the others use gamepads (SDL3
  gamepads, and the browser's Gamepad API via SDL3/Emscripten).
- Each split view renders at a proportionally lower internal resolution, so
  the cost stays roughly constant.
- Split screen and online can be combined (2 local + 2 remote).

## Online

### Topology
**Host-authoritative listen server**: one player's game is the host and runs
the authoritative simulation; others are clients. There are no dedicated game
servers (friends, ≤ 8 players, no costs).

### Transports
| Transport | Platforms | Use |
|---|---|---|
| **WebSocket relay** ("Sleigh Relay") | all (web + desktop) | default for online and cross-play; works through NAT and on gh-pages |
| **WebRTC data channels** (unreliable, unordered) | web ↔ web (later desktop) | phase 2: lower latency peer-to-peer; the relay does the signalling and is the fallback |
| **UDP (ENet)** | desktop ↔ desktop on LAN | "LAN party mode", with LAN discovery — very much in theme |

The **Sleigh Relay** (`tools/relay/`) is a tiny program (a few hundred lines)
that manages **rooms** and forwards packets between room members. It can run
on any small server (a free tier of a container host or a cheap VPS). It
only sees opaque game packets; it stores nothing. GitHub Pages can't host it
(static hosting only), so it's the one piece that needs separate hosting.

### Lobby & joining
- The host creates a room and gets a **room code** like `XMAS-7F3K` and an
  **invite link** `https://stbange.github.io/xmas2026/?join=XMAS-7F3K`.
- Opening the link in a browser joins directly; desktop players type the code.
- Optional room password. Max 8 players (4 in co-op story).

### Netcode
- Simulation at **60 Hz** fixed tick on the host.
- Clients send **inputs** (with tick numbers) at 60 Hz, bundled redundantly
  (last 3 inputs per packet against loss).
- Host sends **snapshots** at 20–30 Hz: delta-compressed against the last
  acknowledged snapshot, quantised positions/angles, relevance filtering
  (only nearby/visible entities).
- **Client-side prediction** for the local player's movement and weapons,
  with reconciliation on mismatch.
- **Interpolation** of other entities (~100 ms buffer).
- **Lag compensation** on the host for hit-scan weapons (rewind up to 200 ms),
  important for day 7.
- Level-specific big simulations (day 10 crowds, day 23 armies) send
  compact unit states with lower update rates for distant units.
- Protocol is versioned; mismatched builds get a friendly error message.

### Progress in co-op
- The **host's calendar** decides which portals are open (see
  [the hall](02-the-hall.md#clock--cheating)).
- Every player keeps **their own save**: finishing a level in co-op counts
  for everyone who reached the end; collectibles count per player who was
  present when they were collected.
- Joining in the middle of a level spawns you at the last checkpoint.
- If the host leaves, the session ends gracefully back to the hall (host
  migration is out of scope).

### Cross-play
Web, Windows and Linux can play together over the relay. WebXR players are
normal web clients; other players see their avatar's head and hands.

## Social extras
- **C-H-E-E-R quick chat** (from day 9): a menu of short lines
  ("Shazbot!", "Merry Christmas!", "Need ammo!", "gg").
- Spray tags (day 7) and friends' best times on the LAN-corner screens.
- No voice chat (players use their own: Discord, Signal, …).

## Security & privacy
- The relay sees only room codes and opaque packets; no accounts, no logs of
  content; rooms expire after inactivity.
- The packet parser is fuzzed (see [quality](08-quality-and-testing.md)).
- Personal data (crew names, photos) is never sent unless the host's
  personalisation pack is loaded; then names are shared with the session.
