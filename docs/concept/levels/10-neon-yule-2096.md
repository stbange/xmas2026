# Day 10 — Neon Yule 2096
*Homage to **Syndicate Wars** (Bullfrog, 1996)*

## The original
- **Gameplay:** cyberpunk squad tactics in a fully 3D city that you can
  rotate and zoom. You control **four cyborg agents** of the EuroCorp
  syndicate, or of the cult Church of the New Epoch. The **Persuadertron**
  turns civilians, police and even enemy agents into followers. The
  **UTOPIA chip** controls the population. Buildings and vehicles are
  **destructible**. Weapons: Uzi, **minigun**, laser, pulse laser,
  **Graviton gun**, electron mace, long-range rifle, knockout gas, razor
  wire, and the orbital strike **Satellite Rain**. Agents are upgraded with
  body mods and research.
- **Look:** an isometric 3D city in neon night and rain, lots of
  explosions, full destructibility.
- **Memorable:** dozens of persuaded civilians running behind your agents;
  dropping a building with Satellite Rain.

## Homage concept
In the megacity of **Neon Yule 2096**, the corporation **JollyCorp** keeps
its citizens happy with the **CAROL chip** (Cerebral Allegiance Regulator,
Obedience Level). KRAMPUS.EXE has taken over the chip network and makes
everyone sing the same jingle forever. Lead a squad of four agents to
the JollyCorp tower, free the citizens, take the bauble from the CEO's office.

**Glitch Lieutenant:** the **JollyCorp CEO** in an exo-suit on the roof of the
tower, protected by persuaded bodyguards.

## Signature mechanics
1. **Squad & drone-cam.** You are agent 1; three AI agents (or co-op friends)
   follow you. Tap *Drone-cam* to switch to an overhead, rotatable view of the
   area (time slows to 25 %) and give orders: move here, attack that, hold.
   It's the Syndicate view, and you then drop back into first person.
2. **Persuadertron.** Hold it on a civilian to make them a follower; they
   pick up weapons and fight for you. Police need 4 followers in your group,
   enemy agents 8. Freed citizens also count toward the Nice alignment (see
   day 17).
3. **Destructible city & Satellite Rain.** Voxel buildings and vehicles break
   apart. In act 3 you designate a target for an orbital strike that
   destroys a whole voxel building.
4. *Bonus:* the **Sugar/Spice/Caffeine** dials (homage to the IPA levels):
   boost speed, accuracy or reaction for your squad, with a "crash" later.

## Arsenal
| Loaner | Notes |
|---|---|
| **Persuadertron** | the signature tool (trophy) |
| **Minigun** | spin-up, devastating |
| **Graviton gun** | chain lightning between enemies |
| **Satellite Rain designator** | once, in act 3 |
| Sugar/Spice/Caffeine dials | squad buffs |

## Enemies
- **JollyCorp police**, **corporate agents** in trench coats, **cyborg
  bodyguards**, **police hover-cars**, the CEO's exo-suit.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing on a corporate terminal with a rotating city map. |
| 0:30 | **Act 1:** rainy street; squad tutorial; drone-cam orders; the first police patrol. |
| 5:00 | Persuade a group of citizens; they arm themselves. |
| 8:00 | **Act 2:** the market district: persuade enemy agents; cut the CAROL chip relay (destroy a relay building). |
| 12:00 | **Act 3:** the tower plaza; Satellite Rain on the barracks; storm the tower with your crowd. |
| 16:00 | **Finale:** roof: the CEO's exo-suit; your persuaded crowd joins the fight. The bauble is in the CEO's safe. |

## Secrets & challenge
- Floppies: in a noodle bar; on a moving monorail; in a building you have to
  blow open.
- **Golden Joystick:** "Pied Piper": reach the roof with 30 followers.

## Look & sound
- Palette: neon magenta/cyan, wet black asphalt, sodium orange.
- Rain particles, wet reflective streets (screen-space reflections at low
  res), many neon dynamic lights; **voxel** buildings that break into chunks.
- Music: dark, melancholic synth with festive chimes.

## Multiplayer
- Co-op: each player is an agent with their own followers; drone-cam orders
  only apply to AI agents.
- Versus (bonus): "Persuasion": two teams persuade the most civilians in 8 minutes.

## Memory
> Someone persuaded every single civilian in the city and then forgot where
> the mission objective was.

## Test hooks
- Persuade thresholds unit tests (civilian/police/agent).
- Voxel destruction: connectivity and debris tests; memory caps.
- Crowd performance test: 60 followers + squad at 60 fps on the reference
  device (perf test in CI with a frame-time budget on the headless sim).

## Research checklist
- First EuroCorp missions; persuadertron behaviour; minigun; Satellite Rain.
