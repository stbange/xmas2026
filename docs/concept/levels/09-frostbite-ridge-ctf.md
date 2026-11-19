# Day 9 — Frostbite Ridge CTF
*Homage to **Starsiege: Tribes** (Dynamix, 1998)*

## The original
- **Gameplay:** team-based online shooter on huge outdoor maps. Everyone has
  a **jetpack** fed by an energy bar. **Skiing** (originally a physics quirk:
  jumping repeatedly on downhill slopes to keep momentum) became the core of
  movement. **Capture the Flag** between team bases. **Armour classes**
  (light, medium, heavy) chosen at **inventory stations**. Bases have
  **generators** that power turrets, force fields and stations; take down a
  generator and the base goes dark. **Deployables** (turrets, sensors, an
  inventory station), vehicles. Quick voice binds ("V-G-S" menu: "Shazbot!").
- **Weapons:** the iconic **Spinfusor** (disc launcher: slow, big splash,
  you could **disc-jump**), chaingun, grenade launcher, mortar, laser rifle,
  ELF gun.
- **Look:** big rolling heightfield terrain with distance fog, angular
  armours, glowing discs.
- **Memorable:** the speed, the mid-air disc hits, the "Shazbot!" voice key.

## Homage concept
KRAMPUS.EXE's bot tribe, the **Shazbots**, has stolen the **Christmas Flag**
from the North Pole's tribe and holds it in a fortified base on Frostbite
Ridge. Ski down, take it back, and hold the line.

**Glitch Lieutenant:** the Shazbot **heavy-armour captain** who carries a
mortar and guards the generator room.

## Signature mechanics
1. **Jetpack & skiing.** Energy bar for the jetpack (shared with energy
   weapons). Hold the *Ski* button on a slope to slide without friction; skiing
   and jetting at the bottom of a hill is how you go fast. Tutorial course
   with speed gates.
2. **Spinfusor & mid-airs.** The *Disc-o Launcher*: slow discs with a big
   splash; disc-jump for height; a **"MID-AIR!"** bonus and sound when you hit
   a flying enemy.
3. **Generators, stations & CTF.** The enemy base's generator powers turrets,
   force fields and its inventory station. Destroy it, or sneak in when it's
   down. At your **inventory station** you switch between light, medium and
   heavy armour (speed vs health vs weapons).

## Arsenal
| Loaner | Notes |
|---|---|
| **Disc-o Launcher** | spinfusor homage (trophy) |
| **Chaingun** | spin-up, close range |
| **Mortar** | heavy armour only |
| **Repair pack** | fixes generators and turrets |
| Deployable turret, inventory station | items |

## Enemies
- **Shazbots** in light/medium/heavy armour: they ski, jet and shout
  quick-chat lines ("Shazbot!", "Incoming!", "Defend our flag!"). Named after
  the crew when a personalisation pack is present.
- Base turrets; mortar emplacements.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a terrain fly-over with the base locations. |
| 0:30 | **Act 1:** ski school: a slalom of speed gates on the ridge; jetpack energy; disc-jumping. |
| 5:00 | Light raid on an outpost: disc combat, first mid-air. |
| 8:00 | **Act 2:** sabotage: sneak in as a light armour, destroy the generator (or repair your own while bots attack it). |
| 12:00 | **Act 3:** the heavy captain; switch to heavy armour, mortar duel. |
| 15:00 | **Finale:** grab the flag and **ski home**, a long, fast downhill chase with the whole tribe after you. Cap → bauble. Then a short defense of the base for 1 minute. |

## Secrets & challenge
- Floppies: on the highest peak (needs a perfect ski-jet line); under the
  enemy base; inside a floating rock.
- **Golden Joystick:** "Speed of Light": the flag run in under 40 s
  (top speed > 200 km/h).

## Look & sound
- Palette: blue-white glaciers, violet dusk sky, energy cyan.
- Big heightfield terrain (shared with day 2), fog, long shadows at dusk;
  glowing disc projectiles with dynamic light.
- Music: driving, orchestral-electronic.

## Multiplayer
- **Versus (headline mode):** CTF for 2–8 players with bots filling the
  teams, online and split screen.
- Co-op: the story run with 2–4 friends against the Shazbots.
- The **C-H-E-E-R quick-chat menu** (V-G-S homage) works in all levels from now on.

## Memory
> We invented our own rule: whoever shouted "Shazbot" the loudest after dying
> had to fetch the next round of drinks.

## Test hooks
- Ski physics tests (friction off on slope, no energy cost, speed caps).
- Generator power graph tests (generator down → dependants off).
- Bot vs bot CTF: 20 headless matches without stuck flags.

## Research checklist
- Raindance, Broadside maps (flow only); disc speed and splash; energy
  values; armour stats.
