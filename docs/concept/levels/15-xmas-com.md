# Day 15 — XMAS-COM: Yule Defense
*Homage to **X-COM: UFO Defense / UFO: Enemy Unknown** (Mythos Games / MicroProse, 1994)*

## The original
- **Gameplay:** two layers. The **Geoscape**: a rotating globe where you
  build bases, research alien technology, intercept UFOs and keep funding
  nations happy. The **Battlescape**: **turn-based** tactical combat with
  **Time Units** (TUs) for every action, **reaction fire**, **fog of war**,
  night missions with flares, **destructible terrain**, and **permadeath**
  soldiers you name and grow attached to. **Research** turns captured alien
  tech into plasma weapons. Alien **psionics** mind-control your soldiers.
- **Enemies:** Sectoids, Floaters, Snakemen, Mutons, **Chryssalids** (turn
  civilians into zombies), Ethereals (psionics); terror missions; Cydonia.
- **Look:** an isometric, voxel-based tactical view with destructible
  buildings; dark, tense music.
- **Memorable:** losing your favourite soldier, the first Chryssalid, the
  motion scanner.

## Homage concept
**U**nidentified **F**estive **O**bjects are landing all over the world.
KRAMPUS.EXE's alien allies, the **Grey Elves**, abduct reindeer. You lead
the **XMAS-COM** squad — soldiers named after your friends — against a
landed UFO and a terror attack on a Christmas market.

**Glitch Lieutenant:** the **Ether-Elf**, a psionic alien commander who
mind-controls your soldiers.

## Signature mechanics
1. **Time-unit tactical mode.** At any moment, press *Tactical* to freeze
   time: a grid overlay appears, you plan moves and shots for each squad
   member within their **TU budget**, then resume and watch it play out. Enemies
   with TUs left take **reaction fire**. You fight in first person in real time
   otherwise, but the squad is best used in tactical mode.
2. **Permadeath squad.** Four soldiers with names (from the crew list), ranks
   and stats. If they die, they're gone for this level (memorial in the hall),
   and replacements are rookies. Soldiers who survive gain ranks.
3. **Interception & research.** In the briefing, a short **Geoscape**
   mini-game: rotate the globe, intercept the UFO with your jet (a mini air
   battle). Mid-mission, captured **Glitterium-115** from the UFO's engine
   lets your scientist research the **plasma rifle** in 2 minutes.

## Arsenal
| Loaner | Notes |
|---|---|
| **Rifle / heavy cannon** | standard issue |
| **Motion scanner** | shows movement through walls on a grid |
| **Plasma rifle** | after research (trophy) |
| **Blaster launcher** | waypoint-guided missile, set in tactical mode |
| Flares, smoke grenades, medikit | |

## Enemies
- **Grey Elves** (sectoid homage), **Floaters** (hovering cyborg snowmen),
  **Crystalids** (fast, turn civilians into crystal zombies), **Mutonuts**
  (huge, armoured nutcrackers), the **Ether-Elf**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | **Geoscape** briefing: rotate the globe, intercept the UFO (90 s). |
| 2:00 | **Act 1:** the crash site at night: flares, the squad, tactical mode tutorial; motion scanner. |
| 7:00 | Breach the UFO; capture Glitterium; research starts. |
| 9:00 | **Act 2:** the **terror mission**: the Christmas market; Crystalids among civilians; save civilians (Nice alignment). Destructible stalls (voxels). |
| 14:00 | **Act 3:** the plasma rifle arrives; the Mutonuts in the church square. |
| 16:30 | **Finale:** the Ether-Elf mind-controls one soldier; break the control (hit the Ether-Elf, or stun your friend). The bauble is in the Ether-Elf's ship. |

## Secrets & challenge
- Floppies: inside the UFO's navigation; under a market stall; on the church roof.
- **Golden Joystick:** "Nobody Dies": finish without losing a soldier.

## Look & sound
- Palette: night blue-black, flare red, alien green, market warm lights.
- Voxel buildings and UFO hull that break; a grid overlay in tactical mode; a
  rotating low-poly globe.
- Music: dark, tense ambient with a creepy music-box Christmas theme.

## Multiplayer
- Co-op: each player controls one soldier; tactical mode is synchronised
  (everyone sees the plan; resume when all are ready).

## Memory
> We named every soldier after someone at the party. When "Kevin" died on the
> first mission, Kevin went to get pizza in protest.

## Test hooks
- TU accounting, reaction fire and planning unit tests.
- Squad persistence (ranks, deaths) and save game.
- Determinism: a planned turn produces identical results on replay.

## Research checklist
- First missions (small scout, terror site); TU costs; reaction fire;
  Chryssalid behaviour.
