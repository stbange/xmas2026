# Day 24 — Code Red-Nose
*Homage to **Command & Conquer: Red Alert** (Westwood Studios, 1996)*

## The original
- **Gameplay:** the prequel to C&C in an **alternate history**: a time machine
  changes the past, and the Allies fight the Soviets in the 1950s. Iconic
  units and buildings: **Tesla coils** (lightning towers that need power),
  the **Chronosphere** (teleports units across the map), the **Iron Curtain**
  (makes units invulnerable for a while), **Tanya** (a commando with dual
  pistols and C4), **attack dogs**, **spies** (disguise as enemies),
  engineers, the **gap generator** (shroud), naval units, MiGs, V2 rockets,
  mammoth tanks. **FMV briefings**, a stirring metal soundtrack.
- **Look:** 2D sprites, red and grey, snowy and Eastern-European maps.
- **Memorable:** Tesla coil sounds, the Chronosphere, Tanya, the music.

## Homage concept
**Christmas Eve.** KRAMPUS.EXE's fortress, the **Krampslin**, sits behind an
**Iron Curtain**. He's about to overwrite NORTHPOLE. NOEL comes back online
(still in safe mode) and gives you the **Chronosphere**. Father Ginger
(from day 3) reappears as KRAMPUS.EXE's general. Everything you've learned in
24 days comes together: you pick **six trophies** from the hall's trophy
wall to take with you, and your saved army from day 23 joins the battle.

The finale: the **Nice/Naughty** choice (see [meta story](../01-meta-story.md)).

**Glitch Lieutenant:** **KRAMPUS.EXE** himself, in his last form: a horned
giant of corrupted voxels in the Krampslin's throne room, protected by the
Iron Curtain.

## Signature mechanics
1. **Chronosphere.** Place a **chrono-beacon**, then chronoshift yourself or
   a target object (a tank, a tesla coil, a bridge piece) to it. Shifted
   vehicles return after 30 s (the "chrono vortex"). Used for puzzles and for
   the assault (shift your army past the wall).
2. **Tesla coils & Iron Curtain.** Tesla coils charge and arc lightning to
   anything near (and chain through water); they need **power**: destroy the
   power plants and they fail. The enemy's **Iron Curtain** makes units
   invulnerable for 10 s (red glow); later you capture it and use it on
   yourself.
3. **Commando, dogs & spies.** You play the **commando** (dual pistols, C4).
   **Attack dogs** sniff out disguised spies. A **spy** disguise (wear a
   Coal-guard uniform) lets you pass checkpoints if you don't run or shoot.
4. **Trophies.** Choose six trophies from earlier levels (Shrinker, Polymorph,
   Disc-o, Persuadertron, Candlesaber, B.F.G. …). The level is designed so
   that many combinations work, with special interactions (e.g. Polymorph a
   tesla trooper, Persuade a dog).

## Arsenal
| Loaner | Notes |
|---|---|
| **Dual pistols** | the commando's |
| **C4** | |
| **Chrono-beacon** | (final trophy: "The Chronosphere") |
| **Spy uniform** | |
| **Six trophies** | chosen in the hall |

## Enemies
- **Coal guards** (rifles), **flame troopers**, **tesla troopers**,
  **attack dogs**, **heavy tanks**, **V2 gift-rockets**, **MiG-sleighs**,
  **tesla coils**, **Father Ginger**, **KRAMPUS.EXE**.

## Level flow (≈25 min — the finale may run long)
| Time | Beat |
|---|---|
| 0:00 | Briefing: FMV-style with NOEL in safe mode and General Holly; trophy selection. |
| 1:00 | **Act 1:** infiltration as a spy through a snowy village; dogs; destroy a power plant with C4: tesla coils go dark. |
| 6:00 | **Act 2:** the battlefield: your army from day 23 (or a default army) attacks; chronoshift tanks past the wall; capture the Iron Curtain. |
| 12:00 | **Act 3:** the Krampslin's interior: tesla corridors, Father Ginger's last stand (he escapes again… into the epilogue as a gingerbread cookie). |
| 17:00 | **Finale:** the throne room. KRAMPUS.EXE behind the Iron Curtain; use the captured Iron Curtain and the Chronosphere to reach him. When he's down: **the choice** (Invite or Delete). |
| 22:00 | **Epilogue** in the restored hall and the 2001 LAN party; credits. |

## Secrets & challenge
- Floppies: in a church tower; in a submarine pen; in the throne room's
  "Recycle Bin".
- **Golden Joystick:** "Every Trophy Counts": use all six chosen trophies in one run.

## Look & sound
- Palette: snow white, Soviet-style red (here Christmas red), grey concrete,
  tesla blue.
- Tesla lightning with dynamic lights, chrono-shift effects (a blue
  shimmer), voxel tanks and buildings, big explosions.
- Music: a stirring metal march with sleigh bells ("Sleigh March").

## Multiplayer
- Co-op: up to 4 players, each with their own six trophies.
- The epilogue is shared: all players are at the LAN party.

## Memory
> Sunrise. Everybody is asleep on the floor except two people still playing
> Red Alert, whispering "one more game".

## Test hooks
- Chrono-shift: valid target positions, return timer, no telefrag
  softlocks.
- Power graph (tesla coils powered/unpowered).
- Trophy matrix: every one of the 24 trophies can be used in the level
  without crashing (game test with each trophy).
- Both endings reachable; "restore from Recycle Bin" path.

## Research checklist
- Allied and Soviet campaign missions (Tanya, chronosphere, iron curtain);
  tesla coil timing; FMV briefing style.
