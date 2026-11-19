# Day 21 — Ages of Advent
*Homage to **Age of Empires** (Ensemble Studios, 1997)*

## The original
- **Gameplay:** historical RTS through four **ages**: Stone, Tool, Bronze and
  Iron. **Villagers** gather **food** (berries, hunting, fishing, farms),
  **wood**, **stone** and **gold**; ageing up unlocks better units and
  buildings. **Priests** **convert** enemy units with a now-famous chant. **Wonders**: build one
  and defend it for a while to win. **Artefacts and ruins** to capture.
  Units: clubmen, slingers, bowmen, chariots, war elephants, catapults,
  hoplites, triremes.
- **Look:** bright, detailed 2D isometric sprites, lots of animals, blue sea.
- **Memorable:** the conversion chant, the cheat codes, the elephants.

## Homage concept
With NOEL in safe mode, the hall is on its own. KRAMPUS.EXE has pushed one
corner of the memories back into prehistory. You lead the small tribe of
**Hollowfolk** through four ages, from the **Ice Age** to the **Iron Star
Age**, against KRAMPUS.EXE's **Coalhorn tribe**, and build a **Wonder**: a
giant Christmas tree. In the hall, each age-up rebuilds a piece of the hall.

**Glitch Lieutenant:** the Coalhorn **war-mammoths** and their chief, who
tries to destroy your Wonder during its countdown.

## Signature mechanics
1. **Ageing up.** Four ages; each age-up (resource cost + a short wait) changes
   your weapon (club → sling → bronze sword → iron bow), your village's look,
   the music, and your allies' units. The level visibly modernises around you.
2. **Gathering & villagers.** Gather by hand (berries, wood, stone, gold) or
   assign villagers (squad module) to gather; drop-off points; build houses,
   a barracks and towers on plots.
3. **"Fa-la-la" conversion & the Wonder.** Your **priest's staff** converts
   enemies with a chant: *"Fa-la-la-la-laaa…"* (a few seconds; interrupted by
   damage). Converted units fight for you. Finally, build the **Wonder** and
   defend it for **3 minutes** (countdown).

## Arsenal
| Loaner | Notes |
|---|---|
| Club → Sling → Bronze sword → Iron bow | age-dependent |
| **Priest's staff** | conversion (trophy) |
| Villager orders, build plots | economy |

## Enemies
- Coalhorn **clubmen**, **slingers**, **chariots**, **war-mammoths**,
  **catapults**, **coal priests** (who try to convert *your* villagers!).

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a narrated history-book page. |
| 0:30 | **Ice Age:** gather berries and wood with a club; hunt; build houses. |
| 4:00 | **Tool Age:** sling; the first Coalhorn raid; towers. |
| 8:00 | **Bronze Age:** sword; capture the ruins and the artefact; the priest's staff; convert a war-mammoth! |
| 13:00 | **Iron Star Age:** bow; the Coalhorn army with catapults. |
| 15:00 | **Finale:** build the Wonder (a giant tree); 3-minute countdown defence; the bauble crowns the Wonder. |

## Secrets & challenge
- Floppies: on a fishing island; inside the ruins; behind a waterfall.
- **Golden Joystick:** "Pacifist Prophet": convert 15 units.
- Easter egg: typing classic-style cheat phrases at NOEL's terminal gives
  silly results (our own phrases).

## Look & sound
- Palette: bright and sunny, Mediterranean-meets-Nordic; snow arriving with
  each age.
- Voxel buildings that upgrade per age; many animals; a blue sea with
  triremes.
- Music: world-music instruments growing into a full orchestra with each age.

## Multiplayer
- Co-op: shared economy; one player can be the priest.
- Versus (bonus): "Wonder race".

## Memory
> We discovered the cheat codes and had a sports car with a rocket launcher in
> the Bronze Age. Nobody remembers who won.

## Test hooks
- Age-up state machine (costs, unlock lists, weapon swaps).
- Conversion timer, interruption, ownership change.
- Wonder countdown; failure if destroyed.

## Research checklist
- First campaign missions; conversion behaviour and timing; Wonder timer.
