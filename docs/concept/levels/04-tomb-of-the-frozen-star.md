# Day 4 — Tomb of the Frozen Star
*Homage to **Tomb Raider** (Core Design, 1996)*

## The original
- **Gameplay:** third-person action-adventure. Levels are built on a
  **grid of square blocks**, and jumps are measured in blocks: standing jump,
  running jump, **grab ledges**, shimmy, climb, back-flip, swan dive.
  **Push/pull blocks**, levers, timed doors, keys and artefacts.
  **Swimming** with an air meter. Combat with dual pistols (infinite ammo)
  that **auto-lock** onto targets; shotgun, magnums, Uzis. Small and large
  medipacks. On PlayStation you saved at **save crystals**.
- **Enemies:** wolves, bats, bears, gorillas, **raptors and a T-Rex** (the
  legendary Lost Valley), mummies and Atlantean mutants.
- **Look:** low-poly with per-vertex lighting, a lonely, quiet atmosphere and
  music only at key moments.
- **Memorable:** the T-Rex reveal, the silence, the music sting when you
  discover a secret.

## Homage concept
Beneath a glacier lies the **Tomb of the Frozen Star**, where the first star
of the Christmas tree was hidden ages ago. KRAMPUS.EXE wants it. You play the
explorer and switch to **third-person view** (the only level that does it by default).

**Glitch Lieutenant:** the **Tyranno-Yule**, a tinsel-covered T-Rex in a lost valley
under the ice, and then the tomb's collapse escape.

## Signature mechanics
1. **Grid platforming in third person.** The level is built on a 1 m block
   grid; standing jump 2 blocks, running jump 3, grab, shimmy, climb, back-flip.
   The camera can switch between third person (default here) and first person.
   Jumps are forgiving but readable: the grid is visible in the textures.
2. **Block puzzles.** Push/pull gift-wrapped stone blocks onto pressure
   plates, levers, timed doors, a four-cog puzzle.
3. **Swimming with air.** A flooded temple: an air bar, currents, levers
   under water; air pockets in the ice.

## Arsenal
| Loaner | Notes |
|---|---|
| **Twin pistols** | infinite ammo, auto-lock onto the nearest target (trophy) |
| **Twin Uzis** | found in act 3, limited ammo |
| Small/large medipacks | heal 50/100 % (here the cocoa is a medipack) |
| **Save crystals** | the level's checkpoints (glowing blue baubles) |

## Enemies
- **Frost wolves**, **ice bats**, **a polar bear**, **Velocireindeer** (raptors with antlers),
  the **Tyranno-Yule**,
  **ice mummies** guarding the tomb.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a handwritten journal page. Jump into the crevasse. |
| 0:30 | **Act 1:** ice cave tutorial: jumps, grabs, climbs; wolves and bats. Silence; wind only. |
| 5:00 | **Act 2:** the lost valley under the ice — a hidden tropical bubble with ferns; Velocireindeer. The ground shakes… the **Tyranno-Yule** reveal (music sting). Escape into a narrow passage, then beat it using a ledge where it can't reach. |
| 11:00 | **Act 3:** the flooded temple: swim, air pockets, four-cog puzzle, push blocks across a pressure-plate floor. |
| 16:00 | **Finale:** the star chamber; take the bauble from the pedestal; the tomb collapses; timed escape run with grabs and a swan dive into a pool. |

## Secrets & challenge
- Floppies: each behind a classic block-jump the observant player sees from
  below. Finding one plays the "secret found" music sting.
- **Golden Joystick:** "Purist": finish without using any medipack.

## Look & sound
- Palette: icy cyan and turquoise; warm torch orange; lush valley greens.
- Coarse block geometry, per-vertex lighting plus dynamic torch light with
  shadows; underwater fog and caustics.
- Music: near silence, ambient wind, short orchestral stings.

## Multiplayer
- Co-op: one player may stay in first person. Some puzzles get a "second
  lever" for co-op so both players have something to do.
- Option: race the escape run with ghosts of your friends' best times.

## Memory
> We watched the T-Rex appear for the first time together and all screamed,
> then pretended we didn't.

## Test hooks
- Grid-jump unit tests: standing/running jump distances, ledge grab
  detection, shimmy edges.
- Block puzzle state machine tests; no unsolvable state (a reset lever exists).
- Bot run over the golden path with recorded jump inputs.

## Research checklist
- Peru levels (Caves, City of Vilcabamba, Lost Valley), St. Francis' Folly;
  jump distances, swim speed and air duration.
