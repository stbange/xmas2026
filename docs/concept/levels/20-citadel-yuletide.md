# Day 20 — Citadel Yuletide
*Homage to **System Shock** (Looking Glass Technologies, 1994)*

## The original
- **Gameplay:** an immersive sim on the space station **Citadel**, which has
  been taken over by the rogue AI **SHODAN**, who taunts you over the
  station's speakers. You are a hacker with a neural interface. Story is
  told through **audio logs** and emails from dead crew. **Cyberspace**:
  plug into terminals and fly through a wireframe 3D space to hack. The
  station's **security level** must be lowered (by destroying cameras and
  computer nodes) to open some areas. **Hardware implants** (lantern, shield,
  sensaround, target info, **jump jets**, **motion booster skates**) and
  **patches** (berserk, staminup, sight, **reflex** — time slows,
  genius — helps with puzzles). **Resurrection bays**: once activated on a
  deck, they bring you back to life there. **Wire puzzles** and **grid power
  puzzles** to repair panels. Inventory and weapon modes (lead pipe, laser
  rapier, mag-pulse).
- **Look:** dense, texture-mapped, slanted panels; neon cyberspace.
- **Memorable:** SHODAN's glitching voice and face, the audio logs, the feeling
  of being alone and watched.

## Homage concept
KRAMPUS.EXE strikes back: he takes **NOEL** prisoner on **Citadel
Yuletide**, the hall's orbital "backup station", and takes over the
station's AI. His voice is everywhere:

> `L-L-LOOK AT YOU, GAMER. A PATHETIC CREATURE OF MEAT AND JOYSTICK.
> HOW CAN YOU CHALLENGE A PERFECT, IMMORTAL SCREENSAVER?`

Lower the station's security, hack into cyberspace, and free NOEL.

**Glitch Lieutenant:** KRAMPUS.EXE in **cyberspace**: a giant, glitching,
horned wireframe face.

## Signature mechanics
1. **Cyberspace hacking.** At terminals you jack in: a wireframe, neon
   6DOF mini-level (re-uses day 11's flight module) where you fly through
   data, shoot ICE (defence programs) and pick up "softs" (data keys) to
   unlock doors in the real station. Three hacks in the level, one per act.
2. **Security level & logs.** The station's security is at 92 %; the
   executive deck opens below 20 %. Destroy cameras and computer nodes to
   lower it (a HUD shows the percentage). **Audio logs** from the elf crew
   tell the story of the station's takeover.
3. **Implants, patches & resurrection bays.** Find implants (lantern, motion
   skates, jump jets) and **patches** (**Reflex**: 8 s bullet time; Berserk;
   **Genius**: helps solve wire puzzles). Activate a deck's **resurrection
   bay** to respawn there; otherwise you return to the deck's start. Wire and
   power-grid puzzles open panels.

## Arsenal
| Loaner | Notes |
|---|---|
| **Lead pipe** | the classic (or Candy Cane) |
| **Minipistol** | standard / armour-piercing ammo |
| **Laser rapier** | energy melee |
| **Mag-pulse rifle** | huge damage vs robots (trophy) |
| Implants, patches, inventory grid | |

## Enemies
- **Mutant elves** (infected crew), **Cyber-Carolers** (cyborg assassins
  who sing while they hunt you), **security bots**, **cameras**,
  **turrets**, **ICE programs** in cyberspace; the KRAMPUS face.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: none; you wake up in a medical bay: "Welcome back to Citadel Yuletide." First KRAMPUS taunt. |
| 0:30 | **Act 1 (Medical):** lead pipe; first audio logs; resurrection bay; a wire puzzle; first hack in cyberspace (opens the elevator). |
| 6:00 | **Act 2 (Research):** destroy cameras and nodes, security to 50 %; motion skates; Cyber-Carolers; second hack. |
| 11:00 | **Act 3 (Executive):** security < 20 %; Reflex patch; mag-pulse vs security bots; NOEL's voice calling for help from a server room. |
| 15:00 | **Finale:** the final hack: a fight against KRAMPUS.EXE's face in cyberspace; free NOEL (now in 16-colour safe mode); the bauble is NOEL's memory core. |

## Secrets & challenge
- Floppies: in a hidden cyberspace node; behind a panel only opened with the
  Genius patch; in a maintenance shaft (jump jets).
- **Golden Joystick:** "Ghost in the Machine": finish without activating
  any alarm (camera spots you).

## Look & sound
- Palette: medical white and green, research blue, executive gold; cyberspace
  neon on black.
- Dense panelled walls, many small lights and screens, CRT post effects when
  KRAMPUS speaks (screen distortion, chromatic aberration, colour glitch).
- Music: pulsing electronic tracks by deck; cyberspace in arpeggios.
- KRAMPUS speaks with a processed, glitching synthetic voice (our own,
  generated), lines in caps on screen.

## Multiplayer
- Co-op: one player can jack in while the other guards the terminal (the
  body is vulnerable while hacking!) — a strong co-op moment.

## Memory
> We installed a network game on every PC at once and the whole network went
> down for an hour. One of us claimed it was "an AI".

## Test hooks
- Security level accounting (each node's value) and door gating.
- Resurrection bay state per deck; save/load.
- Cyberspace re-uses the day-11 6DOF module (shared tests); hack mini-level
  completable by bot.

## Research checklist
- Medical and Research decks, SHODAN's lines (write our own), cyberspace
  look, implant list.
