# Day 7 — de_december
*Homage to **Counter-Strike** (Minh Le & Jess Cliffe; Valve, 1999–2001)*

## The original
- **Gameplay:** team-based tactical shooter in **rounds**. Each round starts
  with a short **buy phase**: money from kills, wins and objectives buys
  weapons, armour, grenades and a **defuse kit**. Objectives: **plant or
  defuse a bomb** (de_ maps) or **rescue hostages** (cs_ maps). No respawn
  inside a round. **Recoil patterns**, movement inaccuracy, headshots.
  **Flashbangs** and **smoke grenades**. Radio commands ("Go go go!", "Fire
  in the hole!", "Enemy spotted"). **Sprays** (player logos on walls).
- **Maps:** dust, dust2, aztec, italy, office, assault.
- **Look:** Half-Life engine (GoldSrc): lightmapped BSP maps, sand-coloured
  textures, sky boxes.
- **Memorable:** dust2's long A, the AWP, the economy, the tension of a 1-vs-3
  clutch, and of course the bot jokes.

## Homage concept
A snowy desert town, **de_december**, where KRAMPUS.EXE's **Krampus Krew**
(bots named after your friends via the personalisation pack) have taken elf
hostages and planted a "coal bomb" under the town's Christmas tree. Five
rounds against bots on one map, alternating objectives, with a buy menu.
This is also the first level with a strong **versus** mode.

**Glitch Lieutenant:** the Krew's star player, **"xX_KRAMP_Xx"**, an
annoyingly good sniper bot who appears in the last round.

## Signature mechanics
1. **Rounds & economy.** 5 rounds. Each starts with a 15 s buy phase in the
   buy zone; money from kills, objectives and round results (+ a loss bonus).
   You keep weapons you survive with. NOEL plays the role of the radio.
2. **Plant/defuse & hostages.** Round types: rescue hostages (they follow you
   with Use), defuse (5 s, 10 s without a kit), retake, and a final "plant"
   round where you're the attackers.
3. **Recoil, flash & smoke.** Fixed, learnable recoil patterns; accuracy
   depends on movement; flashbangs white out the screen (an accessibility
   option tints instead); smoke is a volumetric particle cloud that blocks
   sight *and* light.

## Arsenal (buy menu)
| Item | Price | Notes |
|---|---|---|
| **Krampov-47** | $2500 | accurate first shot, strong recoil |
| **M4 Mistletoe** | $3100 | silencer option |
| **AW-Peppermint** | $4750 | sniper, one shot (trophy) |
| **Desert Reindeer** | $650 | heavy pistol |
| **MP-Snow** | $1500 | SMG |
| Kevlar + helmet, defuse kit, HE / flash / smoke | | |

## Enemies
- The **Krampus Krew**: 5 bots with personalities (rusher, camper, sniper,
  lurker, "the one who always buys a shotgun"), named after the crew.
- xX_KRAMP_Xx (round 5).

## Level flow (≈20 min, ~3–4 min per round)
| Round | Type |
|---|---|
| Briefing | Team-select screen, map overview, controls. |
| 1 | **Hostage rescue**: 4 elf hostages in the workshop; pistols only (economy tutorial). |
| 2 | **Defuse**: the coal bomb is planted at site A or B; retake with your squad of 2 bots. |
| 3 | **Eco round** (scripted low money): a pistol round with smoke and flash tutorial. |
| 4 | **Assault**: storm the Krew's bunker. |
| 5 | **Plant**: you're the attackers; plant on the Christmas tree site and hold against xX_KRAMP_Xx. Win → bauble. |

Losing a round costs time, not the level: you need 3 round wins out of up to
7 rounds (rounds 6–7 replay type 2/5).

## Secrets & challenge
- Floppies: behind a breakable vent; on top of a roof via a boost on a crate;
  a hidden room behind a "dust" poster.
- **Golden Joystick:** "Ace": kill all five bots in a single round.

## Look & sound
- Palette: sand, snow, sky blue, terracotta; lightmapped look, one sunlight
  shadow, sharp sky box.
- **Sprays:** each player sprays their tag (from the personalisation pack).
- Sound: footsteps matter (material-based); radio lines in NOEL's voice; a
  minimal tension score between rounds only.

## Multiplayer
- **Versus (headline mode):** 2–8 players, teams of up to 4 (bots fill up),
  bomb or hostage rules, online and split screen.
- Co-op: friends on the same team against the Krew.
- Lag compensation (host rewind) for hit-scan weapons — see
  [multiplayer](../06-multiplayer.md).

## Memory
> Round 14, tied match, the fuse blew. Nobody ever admitted plugging in the
> kettle.

## Test hooks
- Round state machine tests (buy phase, timers, win conditions, money).
- Recoil pattern determinism; spray-decal placement.
- Bot vs bot simulation: 100 rounds headless, both teams win some rounds
  (balance sanity), no stuck bots.

## Research checklist
- dust/dust2/italy/office layouts as *flow references* (no copying);
  1.3–1.6 economy values; radio lines (record our own).
