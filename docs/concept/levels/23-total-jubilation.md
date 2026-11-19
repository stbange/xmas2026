# Day 23 — Total Jubilation
*Homage to **Total Annihilation** (Cavedog Entertainment, 1997)*

## The original
- **Gameplay:** the first major RTS with **3D units on 3D terrain** and
  real **physics projectiles**: shells fly ballistic arcs, terrain blocks line
  of sight and shots, units can dodge. Two factions (Arm and Core), both
  machine armies. The **Commander** unit builds with a **nanolathe** beam and
  has the **D-gun** (disintegrator, one shot kills anything, costs a lot of
  energy). If your Commander dies, you lose. A **flow economy**: **metal**
  and **energy** are rates (metal extractors, solar, wind, tidal), and you
  **reclaim** wreckage and trees for metal. Wrecks stay on the battlefield.
  Huge armies, Big Bertha artillery, nukes, the giant Krogoth.
- **Look:** low-poly 3D units with team colours on pre-rendered 3D terrain.
- **Music:** a full orchestral score (Jeremy Soule).
- **Memorable:** hundreds of units, the Commander's D-gun, wrecks everywhere,
  the music.

## Homage concept
Christmas Eve is tomorrow. KRAMPUS.EXE's machine army, the **Coal Corps**, is
massing to overwrite NORTHPOLE. You become the **Commander**: a walking
factory in a big armoured suit. Build your army and defend the hall's
last memory sectors; your army carries over (as a bonus) to day 24.

**Glitch Lieutenant:** the **Krampoth**, a gigantic walking machine.

## Signature mechanics
1. **The Commander: nanolathe & D-gun.** You build structures and units with
   the nanolathe beam on free placement (a ghost preview on the terrain, valid
   in green). The **D-gun** destroys anything in one shot but uses 500 energy.
   If you die, the mission fails (checkpoints restore your base).
2. **Flow economy & reclaim.** Metal and energy are **rates** (per second)
   shown as bars with income/expense; build metal extractors on deposits,
   solar collectors and wind generators (wind changes!). Reclaim wrecks and
   trees with the nanolathe for metal.
3. **Ballistic physics & big battles.** All projectiles are physical (arcing
   shells, slow missiles) and can be dodged; hills block shots; craters deform
   the terrain. 100+ units fight at once (your allies and the Coal Corps).

## Arsenal
| Loaner | Notes |
|---|---|
| **Nanolathe** | build/reclaim/repair beam |
| **D-gun** | disintegrator (trophy) |
| **Commander laser** | primary weapon |
| Buildable: metal extractor, solar, wind, factories (bots/tanks), laser towers, Big Bertha | |

## Enemies
- The **Coal Corps**: scout bots, **Stokers** (flamethrower bots),
  **Bunker tanks**, **artillery**, **flying Coal-hawks**, a **Big Bertha**
  homage, the **Krampoth**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: an orchestral hit, a holographic map. |
| 0:30 | **Act 1:** build the economy: extractors, solar, a bot factory; reclaim wreckage from a past battle. |
| 5:00 | First waves: laser towers, your bots; dodge artillery shells. |
| 9:00 | **Act 2:** wind changes; expand to a second metal field; tank factory. |
| 13:00 | **Act 3:** an all-out battle in a valley: 100+ units; capture the Coal Corps' Big Bertha hill. |
| 16:00 | **Finale:** the Krampoth walks in; D-gun it at close range (energy must be full!). The bauble is in its core. The units you have left are "saved" for day 24. |

## Secrets & challenge
- Floppies: in an old wreck (reclaim it); on a cliff; under a crater.
- **Golden Joystick:** "Commander's Honour": finish without the Commander
  dropping under 50 % health.

## Look & sound
- Palette: green-brown hills, metal grey, team colours: festive red vs. coal black.
- Low-poly 3D units (GPU instancing), persistent wrecks, deformable terrain
  craters, dynamic lights from lasers and explosions.
- Music: big orchestral score with festive themes.

## Multiplayer
- Co-op: each player is a Commander with their own economy.
- Versus (bonus): 1-vs-1 "Commander duel".

## Memory
> A game that started at 1 AM lasted until breakfast. The two last players
> agreed to a draw because the pizza was getting cold.

## Test hooks
- Flow economy unit tests (rates, stalls when negative, storage caps).
- Ballistic projectile tests (arc, terrain collision, deformation).
- Performance test: 150 units at 60 fps on the reference device; headless sim
  budget in CI.

## Research checklist
- Arm/Core campaign first missions; economy values; Commander behaviour;
  D-gun.
