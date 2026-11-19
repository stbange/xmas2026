# Day 11 — Descent into the Gingerbread Mines
*Homage to **Descent** (Parallax Software, 1995)*

## The original
- **Gameplay:** a fully 3D shooter with **six degrees of freedom**: you fly a
  small ship through twisting mine tunnels, with no "up". Hostile mining
  **robots**. **Rescue hostages** (miners). Blue/yellow/red **keycards**.
  At the end, destroy the **reactor** and then escape in a **countdown**
  before the mine explodes. A rotating 3D wireframe **automap**. Descent II
  added the **Guide-Bot** (shows the way) and the **Thief-Bot** (steals
  your weapons), and the afterburner.
- **Weapons:** lasers (upgradable levels), vulcan cannon, spreadfire,
  plasma, fusion; missiles: concussion, **homing**, smart, mega; **flares**.
- **Look:** texture-mapped polygons, coloured light, sprite powerups.
- **Memorable:** losing orientation, the reactor countdown, motion sickness.

## Homage concept
Under the North Pole are the **Gingerbread Mines**, where the elves dig sugar
crystals. KRAMPUS.EXE has infected the mining robots, the miner elves are
trapped, and the mine's **sugar reactor** is going critical. Fly the **Pyro-Elf**
mining ship in, rescue the miners, destroy the reactor, and get out.

**Glitch Lieutenant:** the **Mega-Nutcracker**, a giant robot guarding the
reactor core.

## Signature mechanics
1. **6DOF flight.** Full pitch/yaw/roll, strafing in all directions, an
   afterburner. Comfort options: auto-level (gently rolls you back to the
   level's "up"), cockpit frame, vignette, snap-roll.
2. **Reactor countdown & hostages.** Rescue miner elves (fly into their
   cell) and keep them on board until the exit. After the reactor dies, a
   **50-second** escape with flashing red lights and alarms; the exit route is
   shown on the HUD.
3. **Flares, homing missiles, Guide-Bot & Thief-Bot.** Dark sections need
   flares (dynamic lights that stick to walls). Primary/secondary weapons
   (lasers, then homing missiles). The **Pathfinder Puppy** guide-bot
   leads you to the next objective. A **Thief-Bot** steals your best weapon;
   catch it to get it back.

## Arsenal
| Loaner | Notes |
|---|---|
| **Twin lasers** | upgradable with laser boosts (levels 1–4) |
| **Spreadfire** | 3-way spread |
| **Homing missiles** | lock-on secondary (trophy) |
| **Flares** | light the dark |
| Afterburner, Pathfinder Puppy | |

## Enemies
- **Drill bots**, **hopper bots**, **sniper bots** that strafe, **cloaked
  bots**, the **Thief-Bot**, **turrets**, the **Mega-Nutcracker**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a rotating wireframe of the mine; "Pyro-Elf, you are cleared." |
| 0:30 | **Act 1:** flight tutorial in a large shaft; first drill bots; blue keycard. |
| 5:00 | Rescue the first miners; the Pathfinder Puppy joins. |
| 8:00 | **Act 2:** dark sector: flares; yellow keycard; the Thief-Bot steals your spreadfire; chase it through narrow tunnels. |
| 13:00 | **Act 3:** a flooded-with-lava cavern, red keycard; cloaked bots. |
| 16:00 | **Finale:** the reactor core and the Mega-Nutcracker; destroy the reactor; **50 s escape**; the bauble is on the way out (pick it up or lose it!). |

## Secrets & challenge
- Floppies: behind a breakable wall; in a hidden hostage cell; in a secret
  tunnel only visible on the automap.
- **Golden Joystick:** "Miner's Friend": rescue all 8 miners and escape with
  more than 20 s left.

## Look & sound
- Palette: dark gingerbread brown, sugar-crystal blue, lava orange, alarm red.
- Coloured lights, sticky flares with shadows, sprite powerups, 3D wireframe
  automap (also on the NOEL-Pad).
- Music: driving, industrial rock with bells.

## Multiplayer
- Co-op: 2–4 ships; hostages count for the team.
- Versus (bonus): "Anarchy" deathmatch in the reactor caverns.
- XR: the most intense level; marked "Intense" on the comfort rating, with
  cockpit and teleport-free comfort options.

## Memory
> Two people threw up. One of them kept playing.

## Test hooks
- 6DOF physics tests (rotation integration, collision against tunnels, no
  tunnelling at max speed).
- Countdown logic; hostages count; "bauble lost" path still completes.
- Re-used by day 20 (cyberspace): shared module tests.

## Research checklist
- Levels 1–4 of Descent; Descent II guide-bot and thief-bot; countdown times.
