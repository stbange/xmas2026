# Day 13 — Phobos Gift Depot
*Homage to **Doom** (id Software, 1993)*

## The original
- **Gameplay:** the archetypal FPS. Very fast movement, non-linear levels
  with **red/yellow/blue keycards** and **secret doors**, an automap.
  Monsters fight each other if one hurts another (**infighting**).
  Power-ups: **berserk pack** (huge punches), **invulnerability**,
  **partial invisibility**, radiation suit, light-amp visor, computer map,
  soul sphere, mega armour. Weapons: fist, **chainsaw**, pistol, shotgun,
  super shotgun (Doom II), chaingun, rocket launcher, plasma rifle, **BFG9000**.
- **Enemies:** zombiemen, shotgun guys, imps (fireballs), pinky demons,
  spectres (invisible pinkies), lost souls, cacodemons, barons of hell, the
  cyberdemon and spider mastermind.
- **Look:** sprites, 90°-ish walls (no room over room), **sector lighting**
  with flickering and strobing lights, diminishing light into darkness.
  The **status-bar face** that looks around and bleeds.
- **Memorable:** E1M1's music and layout, the shotgun, the BFG, the face.

## Homage concept
The **Phobos Gift Depot**, the moon base where Christmas gifts are
teleported to Earth, has had a teleporter accident. Demons came through.
And this time, KRAMPUS.EXE himself is behind it: he steps into the game in
person for the first time, as the **Cyber-Krampus**. "Rip and tear… the
wrapping paper."

**Glitch Lieutenant:** **Cyber-Krampus**, KRAMPUS.EXE's avatar with a
rocket-launcher arm — the Act II midpoint boss.

## Signature mechanics
1. **Keycards & secrets.** Classic non-linear layout with red/yellow/blue key
   locks, an automap, and lots of secrets (walls that look slightly different,
   switches that open hidden doors). The level reports secrets found (x/y) at
   the end.
2. **Monster infighting.** If an enemy hurts another, they fight each other.
   The level is built around it: lure Coal Imps into the line of fire of the
   Pudding Demons, and get a pair of Bauble-demons to kill a Baron.
3. **Berserk, chainsaw & the B.F.G.** The *Berserk Gingerbread* makes your
   punches gib demons (screen turns red). The *Tree-Saw*, a chainsaw for
   Christmas trees and demons. The **B.F.G. — Big Festive Gun** — fires a
   green ball of festive cheer that destroys everything in its path.
4. *Bonus:* the **status face** — in this level only, a pixel face of your
   avatar in the HUD looks around and gets bloodier.

## Arsenal
| Loaner | Notes |
|---|---|
| **Shotgun / super shotgun** | the stars |
| **Tree-Saw** | chainsaw |
| **Rocket launcher** | rocket-jumping not needed (that's day 16) |
| **Plasma rifle** | |
| **B.F.G.** | finale only (trophy) |
| Berserk Gingerbread, invisibility, invulnerability | power-ups |

## Enemies
- **Zombie elves** (pistol, shotgun), **Coal Imps** (throw burning coal),
  **Pudding Demons** (charging plum puddings with teeth; invisible variant:
  "spectre pudding"), **Lost Baubles** (flaming skull-ornaments),
  **Bauble-demons** (floating one-eyed ornaments that spit lightning),
  **Barons of Humbug**, **Cyber-Krampus**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a single text screen on a starfield. |
| 0:20 | **Act 1:** the hangar (a homage to the *flow* of E1M1, not its map); zombie elves; shotgun; blue key. |
| 5:00 | Toxic-sludge storage, radiation suit; berserk; first infighting setup. |
| 9:00 | **Act 2:** the teleporter hall: yellow key, dark section with light-amp visor; Tree-Saw. |
| 13:00 | **Act 3:** the corrupted base turns into hell: walls of gift paper and flesh; red key; Barons and Bauble-demons in an infighting arena. |
| 16:30 | **Finale:** Cyber-Krampus in an open arena; use the pillars. The B.F.G. is found just before. The bauble drops; KRAMPUS.EXE logs off swearing. |

## Secrets & challenge
- Floppies: a classic hidden door behind a texture that's slightly
  different; an outdoor ledge from a hidden lift; a "secret exit" room.
- **Golden Joystick:** "UV-Max": 100 % kills and 100 % secrets.

## Look & sound
- Palette: brown-green tech base, toxic green, hell red; strong sector-light
  banding (diminishing light done with the palette light ramp).
- Pure sprite monsters (8 angles), flickering and strobing lights (with
  photosensitivity toggle), skybox of Mars.
- Music: metal riffs played as MIDI-style synth, with sleigh bells.

## Multiplayer
- Co-op: 1–4 with more monsters per player. Versus (bonus): "Deathmatch 2.0"
  on the teleporter hall.

## Memory
> The first deathmatch over a null-modem cable, years before this party. We
> had come a long way: now there were eight of us.

## Test hooks
- Infighting rules unit tests (who retaliates against whom; same-species rules).
- Key-lock graph check: every locked door is reachable with keys obtainable
  before it (static analysis of the level).
- Secret counter; automap reveal.

## Research checklist
- E1M1–E1M3, E1M8; infighting rules; berserk; sector light effects.
