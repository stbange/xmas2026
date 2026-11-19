# Day 16 — The Elder Yule
*Homage to **Quake** (id Software, 1996)*

## The original
- **Gameplay:** the first truly 3D FPS. Its **start map** is a hub: you pick
  difficulty and episode by walking through gates. Each episode ends with a
  **rune**; four runes open the final gate. Movement tricks grew from it:
  **rocket jumping**, **strafe-jumping** and **bunny hopping**. Power-ups:
  **Quad Damage**, Pentagram of Protection, Ring of Shadows, Biosuit.
  Weapons: axe, shotgun, double-barrel, **nailgun**, super nailgun,
  grenade launcher, rocket launcher, **thunderbolt** (discharge it under water
  and everyone dies, including you). **Zombies** only die when gibbed.
- **Bosses:** Chthon (a lava lord killed by **electrodes**, not weapons) and
  Shub-Niggurath (killed by a **telefrag**).
- **Enemies:** grunts, rottweilers, knights, death knights, ogres (chainsaw +
  grenades), fiends, shamblers, scrags, vores.
- **Look:** brown, gothic-industrial, lightmaps, true polygon models with
  vertex animation, fullbright pixels; ambient industrial soundtrack.

## Homage concept
The **Elder Yule** is a dimension that looks like our hall, only older,
browner and stranger. Four slipgates lead to four short wings; each holds a
**rune**. With all four runes the last gate opens and you reach the bauble…
and a terrible truth.

**The reveal:** the bauble contains the memory of the night in 2001 when one
of the friends wrote `KRAMPUS.EXE` as a joke screensaver. It was never shut
down properly. It ran alone for 25 years.

**Glitch Lieutenants:** the **Lava Lord of Yule** (electrode puzzle) and
**Shub-Nog-Nog** (telefrag puzzle).

## Signature mechanics
1. **Rocket & strafe jumping.** A short movement course teaches air control
   and rocket jumping (reduced self-damage on *Pizza*/*Cola*). Bunny-hopping
   works and is rewarded.
2. **Rune hub & Quad Damage.** A mini start-map hub with four gates: the
   *Base*, the *Castle*, the *Crypt* and the *Abyss*. Each wing is ~3 minutes
   and ends in a rune. Quad Damage in each wing (with the famous sound).
3. **Puzzle bosses.** The **Lava Lord** can only be killed with three
   electrode pylons (hit the switches while dodging lava balls). **Shub-Nog-Nog**
   can only be killed by teleporting *into* it with the right timing (telefrag).
   Also: the thunderbolt discharges in water.

## Arsenal
| Loaner | Notes |
|---|---|
| Axe → **Candy Axe** | |
| **Double-barrel shotgun** | |
| **Nailgun / super nailgun** | |
| **Rocket launcher** | rocket jumping (trophy) |
| **Thunderbolt** | do not use in water |
| Quad Damage, Biosuit, Ring of Shadows | power-ups |

## Enemies
- **Grunt elves**, **rotten reindeer** (dogs), **tin knights** & **death
  tin knights**, **Ogres of the Oven** (chainsaw + grenade), **fiends**,
  **Shamblers** (big white yeti with lightning), **zombie gingerbread men**
  (gib only), the bosses.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: none — you're dropped into the start-map hub. NOEL is silent (no signal). |
| 0:30 | **Hub:** movement course (strafe-jump gap, rocket jump ledge). |
| 2:00 | **Wings 1–4** (any order, ~3 min each): Base (grunts, nailgun), Castle (knights, ogres), Crypt (zombies, Quad), Abyss (fiends, Shambler). |
| 14:00 | **Lava Lord** (after wing 2, electrode puzzle). |
| 16:00 | **Finale:** the last gate; **Shub-Nog-Nog** telefrag; the bauble and the **reveal** memory (60 s scene). |

## Secrets & challenge
- Floppies: one per wing (3 of 4 wings), behind shootable walls and rocket
  jumps.
- **Golden Joystick:** "Speedrunner": the whole level in under 10 minutes.

## Look & sound
- Palette: famous brown; fullbright orange and blue; lava red.
- Lightmaps plus dynamic light from rockets/muzzle, coloured where we like;
  **vertex-morph animation** for monsters (like old MDL models).
- Music: ambient-industrial drones (no melody), clanks, whispers.

## Multiplayer
- **Versus (headline):** deathmatch on the hub and on a dedicated map, with
  rocket jumping and Quad Damage. Teams or free-for-all, 2–8.
- Co-op 1–4: each player can carry runes; the boss puzzles need everybody.

## Memory (the reveal)
> 03:12 AM. Everyone else is asleep. One of us writes a small program:
> KRAMPUS.EXE — a screensaver that jumps out at whoever falls asleep first.
> It works. Everybody laughs. The PC is switched off at the wall. The program
> is still in memory when the disk is copied.

## Test hooks
- Movement physics tests: air acceleration, strafe-jump speed gain, rocket
  knockback; these are regression-locked (golden values).
- Electrode and telefrag puzzle state tests.
- Water + thunderbolt discharge test.

## Research checklist
- Start map, E1M1, E1M7, E4M8 and END; rocket/strafe jump physics values;
  sound design (make our own).
