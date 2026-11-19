# 00 — Vision & Pillars

## Elevator pitch

> **Advent of LAN** is a first-person advent calendar for grown-up gamers.
> A festive hall holds 24 portals; one opens every day in December. Each
> portal leads into a 20-minute love letter to a game we played on a beige PC
> in 2001: you kick alien boars in a mall, fly a magic sled, pilot a mech,
> defuse a bomb, dive through a mine in six degrees of freedom. A virus called
> **KRAMPUS.EXE** is eating our old LAN-party memories, and you have 24 days to
> get them back.

## Audience

- A circle of friends who are now about 40 and grew up on DOS/Win9x PC games
  (plus a little SNES).
- They know the originals, so the fun is in **recognising** them: "oh, it's
  the Obelisk!", "wait, *that's* the Tesla coil!".
- They have little time: one session a day, about 20 minutes, often on a
  phone, a work laptop or a couch with a gamepad. Some will try it in a VR
  headset because they can.
- They like to play **together**: split screen on a sofa at Christmas, and
  online from different cities.

## Pillars

1. **One door a day.** The calendar is the heartbeat. Every day has something
   new, every level can be finished in one ~20-minute sitting, and every door
   stays open once it has opened.
2. **Homage, not remake.** Each level borrows the *verbs* of its inspiration
   (2–3 signature mechanics plus weapons and items), its mood and its
   palette. It never copies its assets, names or maps. When a friend
   recognises the game within 30 seconds, the level works.
3. **One game, 24 flavours.** The core stays the same in every level: run,
   aim, shoot, use, solve. Players never re-learn the basics, only the
   twist. The mechanics are reusable modules, so day 23 can borrow from day 3.
4. **Retro look, modern feel.** Chunky pixels, palettes, sprites and voxels,
   but with dynamic lights, real shadows, 60 fps, good controls, checkpoints,
   accessibility options and no lost progress.
5. **Better together.** Every level can be played solo, and all levels can be
   played in co-op (split screen or online). A few levels have versus modes,
   because that's what a LAN party is for.
6. **Runs anywhere a friend is.** Browser first (desktop and mobile), plus
   native Windows/Linux builds and WebXR. Sharing a link must be enough.

## Tone

- Warm, nostalgic, funny. Christmas puns are welcome; cynicism is not.
- Gore is cartoonish and can be turned off ("Gore: Off / Confetti / Classic").
- KRAMPUS.EXE is a menace with a sad backstory, not pure evil (see
  [meta story](01-meta-story.md)).

## Scope

| In scope | Out of scope (for 2026) |
|---|---|
| 24 levels × ~20 min, hub hall, meta story, epilogue | Level editor for players |
| Solo, co-op split screen (≤4), online (≤4) | Matchmaking with strangers, accounts, rankings |
| Windows, Linux, Web (desktop + mobile), WebXR | macOS/iOS/Android native apps, consoles, desktop OpenXR |
| Keyboard+mouse, gamepad, touch, XR controllers | Voice chat |
| English UI and text; German translation planned | Voice acting beyond a few processed lines |

## Title & naming

- Working title: **Advent of LAN — 24 Portals to 2001**.
- Engine codename: **Tannenbaum** (`tb_` prefix in C).
- The repository is `stbange/xmas2026`.

## Success criteria

- Every friend finishes at least 20 of the 24 doors in December.
- Each portal is ready and playable on the web at 00:00 CET on its day.
- The opening line of a group chat on 24 December is "who's online?".
