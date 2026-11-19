# Day 3 — Operation Tinselium
*Homage to **Command & Conquer** (Westwood Studios, 1995)*

## The original
- **Gameplay:** the RTS that set the template: harvest **Tiberium** with
  harvesters into **credits**, build from a **sidebar** (construction yard,
  power, refinery, barracks, factory), two factions (GDI and the Brotherhood
  of Nod). Units: rifle infantry, **commando** ("Keep 'em coming!"), flame
  troopers, **engineers** that capture buildings, light/medium/mammoth
  tanks, harvesters. Superweapons: **Ion Cannon**, nuclear strike. Base
  defences: guard towers, **Obelisk of Light** (charges, then fires a deadly laser).
- **Look:** 2D isometric-ish sprites, a sidebar UI, **FMV briefings** with live
  actors and a charismatic bald villain; a computer voice ("Construction
  complete", "Unit ready").
- **Memorable:** the green crystals that poison infantry, the commando, the
  Obelisk, the computer voice.

## Homage concept
Glowing green **Tinselium** crystals are growing across the snowy desert
after a "meteor" (KRAMPUS.EXE's virus payload). The **Brotherhood of Nog**,
led by a smooth-talking gingerbread prophet called **Father Ginger**, harvests
it to turn the world into tinsel. You're the commando of **G.I.F.T.** (Global
Initiative For Tinsel-control). Briefings are delivered by **General
Holly** in grainy "FMV" (low-res video made in the engine with scanlines and
noise). The computer voice is **ELVA**: "Construction complete." "Unit ready."

**Glitch Lieutenant:** the **Obelisk of Frost** guarding Father Ginger's
temple, plus Father Ginger himself escaping in a stealth sleigh.

## Signature mechanics
1. **Credits & build plots.** Shoot Tinselium crystals or escort your
   harvester to earn credits. On your wrist "sidebar" (a radial menu) you buy
   sandbags, guard towers and units, which you place on build plots or call
   in at the barracks/war factory. A sidebar build takes real time with a
   clock-wipe icon, like the original.
2. **Engineers capture.** Lead an engineer squad (squad module) into an enemy
   building to capture it: capture the Nog refinery to steal their income,
   capture a power plant to shut down their defences.
3. **Laser towers & orbital strike.** The Obelisk of Frost charges for 3 s
   (rising tone and glow) before firing: time your dashes between covers. To
   destroy the temple, plant C4 on the generator and then place an **Ion
   Uplink beacon** for an orbital strike.

## Arsenal
| Loaner | Notes |
|---|---|
| **Commando rifle** | one-shot infantry at range |
| **C4 charges** | timed, plant on structures |
| **Ion Uplink beacon** | calls the orbital strike (trophy) |
| Tinselium credits | resource bar |

Tinselium fields hurt the player over time (wear the hazmat suit pickup).

## Enemies
- **Nog acolytes** (rifle infantry), **flame elves**, **recon bikes**,
  **light tanks** (voxel), **Nog harvester** (big, armoured), the **Obelisk of Frost**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | FMV briefing with General Holly (static, scanlines); ELVA online. |
| 0:40 | **Act 1:** drop from a transport; harvest/escort tutorial; buy sandbags and a guard tower. |
| 5:00 | Defend your harvester and refinery against two Nog waves; "Unit ready" — your first squad. |
| 8:00 | **Act 2:** engineers. Capture the Nog refinery (their income is now yours); capture a power plant — the Obelisk's glow dims. |
| 13:00 | **Act 3:** infiltrate the temple valley; Obelisk timing; plant C4. |
| 17:00 | **Finale:** place the Ion Uplink beacon; 30 s countdown; run; the beam hits; the bauble is in the ruins; Father Ginger escapes laughing (sequel bait for day 24). |

## Secrets & challenge
- Floppies: inside a captured Nog barracks; on a Tinselium-infested island
  (needs the hazmat suit); a "lost" FMV clip in the temple.
- **Golden Joystick:** "Commando": finish with ≤3 units built.

## Look & sound
- Palette: sandy beige with snow, Tinselium green glow (emissive), GDI-ish gold.
- Voxel vehicles and buildings (voxels are perfect for this era), sprite
  infantry. Overhead "mission map" camera in the briefing.
- Music: industrial-funk beat with sleigh bells; ELVA voice lines.

## Multiplayer
- Co-op: one player can take the "commander" role and use the sidebar from the
  build terminal's overhead camera, while others fight on the ground.
- Versus (bonus): "Tinsel Rush" — two teams each defend a refinery.

## Memory
> Someone built nothing but harvesters, and somehow won anyway.

## Test hooks
- Economy module unit tests (income rates, build queue, refunds).
- Capture logic: an engineer entering an enemy building switches its team and
  the building's effects (income/power).
- Bot run: reaches the Ion beacon and completes in < 19 min.

## Research checklist
- GDI missions 1–3 and Nod missions; Obelisk timing; ELVA-style voice lines
  (we write our own).
