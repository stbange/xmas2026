# 02 — The Hall & the Advent Calendar

## The Great Hall of Saved Games

A vast festive hall, part gothic cathedral, part 1950s village community hall,
part beige computer case. Snow falls through a broken skylight. The floor is
polished wood with a huge circuit-board inlay.

```
                      N (balcony stairs)
        ┌──────────────────────────────────────┐
        │  13 14 15 16 17 18 19 20 21 22 23 24 │   upper gallery (balcony), doors 13–24
        │                                      │
        │   LAN corner      ╔══════╗   Jukebox │
        │   (CRTs, stats)   ║ TREE ║           │
  W     │                   ╚══════╝           │    E
 (Arena │   Trophy wall      NOEL's      Cocoa │
  doors)│                    desk        bar   │
        │  1  2  3  4  5  6  7  8  9 10 11 12  │   ground floor, doors 1–12
        └────────────────┬─────────────────────┘
                         S (entrance: the "BIOS" gate)
```

### Zones
- **The Tree**: a 30 m voxel Christmas tree in the centre. It starts bare and
  grey. Each bauble you bring back is hung on it and lights up a ring of
  lights. On day 24 KRAMPUS.EXE (or the star) goes on top.
- **The 24 Advent Doors**: big wooden doors in an advent-calendar style,
  numbered with painted numbers, 12 on the ground floor and 12 on the
  balcony. Behind each door is a **portal**: a swirling surface that shows a
  live, low-resolution view into the level (render-to-texture).
- **NOEL's desk**: a 2001 reception desk with a CRT. Daily message, hints,
  options, stats, save slots.
- **The LAN corner**: tables with CRT monitors that show stats: completion
  times, secrets found, frags in versus modes, the friends' best times
  (online).
- **Trophy wall**: one trophy per finished level, a "loaner" weapon or item
  from that level that you can take out for the Snowball Arena or Remix Mode.
- **Jukebox**: plays each finished level's music.
- **Cocoa bar**: heals, and the place where co-op players gather.
- **Snowball Arena** (west wing): a small versus arena for 2–8 players with
  snowballs and trophies. It is open from day 1 so the friends have something
  to play together before the levels unlock.
- **The BIOS gate** (south entrance): start, options, credits.

## The advent unlock

### Rule
Portal *N* (1–24) opens when the current date is **on or after
2026-12-*N* 00:00 in the player's local time zone**. Once open, it stays open.
After 24 December everything stays open forever, and Remix Mode unlocks.

- Before 1 December the hall is playable (arena, exploring, NOEL's countdown).
- A door that is not open yet shows a frosted, nailed-shut advent door with its
  number and a countdown ("opens in 3 d 14 h"). Bumping into it plays a
  wobbly sound and a NOEL quip.
- A door that opened today has a glowing ribbon and a "NEW!" tag.
- Doors can be played in any order once open. Days you missed simply wait.

### Clock & cheating
These are friends, not an e-sports league: the rule uses the local clock.
- In **online** games the **host's** date decides which portals are open for
  everybody in the session.
- The lobby/relay server sends its UTC time when you connect. If the local
  clock is more than 36 hours ahead of the server, NOEL makes a joke
  ("Nice try, Marty McFly") and the server time is used instead. That's all.
- **Playtest builds** (`https://stbange.github.io/xmas2026/playtest/`, debug
  builds; CMake option `TB_PLAYTEST_BUILD`) take a `?playtest=1` URL
  parameter or `--playtest` flag that opens all portals and shows a debug
  overlay, and `?date=2026-12-10` / `--date` to pretend it is another day.
- **Release builds** (the site the friends use,
  `https://stbange.github.io/xmas2026/`, and the native downloads) ignore
  both switches, so nobody can peek ahead or unlock doors early by URL.

### Content drop (spoiler protection, optional)
The web release can deliver levels in daily *content packs*: a scheduled
GitHub Action publishes `level_NN.pak` at 00:05 CET on day *N*. That keeps the
surprise for curious friends who look at the network tab, and lets the
developer finish levels until the day before. The native builds can download
the same packs or ship with all of them included.

## How the hall changes over December
The hall is a story device. Every finished level and every calendar day
changes it:

| Days | Change |
|---|---|
| 1–8 (BOOT) | Bare tree, dim hall, slow snow. Each bauble lights one ring of the tree and switches on one area (jukebox, LAN corner, cocoa bar …). |
| 9–16 (DEFRAG) | Static flickers in the snow, a few textures "corrupt" into noise, a door twitches. KRAMPUS.EXE graffiti appears on walls overnight (in his chat style). |
| 16 | After the Quake-style reveal, the old 2001 photo appears on NOEL's desk. |
| 17–24 (SHUTDOWN) | Red emergency lighting. From day 20 NOEL is in safe mode: a 16-colour sprite. On day 23 the hall is "fortified" for the final war. |
| 24 + epilogue | Fully restored hall, warm light, all decorations on, the star (or KRAMPUS.EXE) on the tree. |

## Hall as a technical showcase
The hall is the first thing everybody sees and it must run on a phone:
- Uses the full lighting system: a moonlight shadow from the skylight, the
  tree's coloured point lights (some with shadows), the fireplace.
- The portals are low-resolution render-to-texture previews (64×64 to 128×128,
  updated round-robin, one per frame) of each level's entry scene.
- The snow is a GPU particle system; on low-end devices it is reduced.

## Remix Mode (after 24 December)
- Every level can be played with any trophies from the trophy wall.
- "Daily Remix": a seeded modifier each day (low gravity, big heads, all
  enemies are day-1 boars, etc.).
- Speedrun timers and online leaderboards between friends.
