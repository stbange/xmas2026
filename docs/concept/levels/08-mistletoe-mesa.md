# Day 8 — Mistletoe Mesa
*Homage to **Half-Life** (Valve, 1998)*

## The original
- **Gameplay:** story told **without cut scenes**: you never lose control,
  starting with a **tram ride** through the research facility. Scripted events
  happen around you. A failed experiment (the "resonance cascade") opens
  the facility to alien invaders. **Scientists** and **security guards**
  follow you and open doors. The **HEV suit** with wall **chargers** for health
  and armour. The **crowbar**. Clever squad AI of the military marines.
  The **long-jump module**. The **tau cannon** (gauss jump!), **gluon gun**,
  **snarks**, tripmines, satchel charges.
- **Enemies:** headcrabs (latch onto heads), zombies, vortigaunts, houndeyes,
  bullsquids, barnacles, marines, the giant tentacles in the blast pit, gargantua.
- **Look:** GoldSrc engine, lightmaps, skeletal animation; industrial labs.
- **Memorable:** the tram ride, the blast pit, the guy in the suit who keeps
  watching you.

## Homage concept
The **Mistletoe Mesa Research Facility** studies anomalous gingerbread. On
the day you arrive, a scientist inserts KRAMPUS.EXE's "sample" into the
analyser, triggering the **Resonance Carol-cade**: carols play backwards and
the facility fills with festive aliens. Survive, reach the surface, take the
bauble from the sample chamber.

This is where the **Man in the Ugly Sweater** is introduced properly: he
watches you from behind the glass of the tram ride, then walks away.

**Glitch Lieutenant:** the **Big Garland**, three giant tinsel tentacles in
a test silo that react to sound.

## Signature mechanics
1. **Seamless scripted story & followers.** A 2-minute tram ride intro
   through the facility (no control except looking around, skippable after
   the first time). Scripted events (a scientist dragged into a vent). **Use**
   a scientist to make them follow you; scientists open retinal-scanner doors;
   guards fight with you.
2. **Long-jump & chargers.** The long-jump module (crouch + jump = a long
   leap) for the silo and canyon gaps. HEV-style wall chargers for health
   and armour, which drain as you use them.
3. **Charge gun & critters.** The *Tau-ment* (tau homage): charge to fire
   through walls and use the recoil to **boost-jump**. *Snarkies*: wind-up
   toy mice that hunt enemies (and you, if they run out of targets).

## Arsenal
| Loaner | Notes |
|---|---|
| **Candy Cane** | the base melee has a starring role here |
| **9mm pistol / SMG** | SMG with grenade launcher |
| **Tau-ment charge gun** | charge, shoot through thin walls, boost-jump (trophy) |
| **Snarkies** | 5 wind-up mice |
| Long-jump module, HEV chargers | items |

## Enemies
- **Hatcrabs** — Santa hats with spider legs that latch onto your head (and turn
  scientists into **jingle-zombies**).
- **Glow-elves** — shoot green lightning.
- **Hazard Elf Commandos** — squad AI: flank, throw grenades, retreat.
- **Big Garland** — boss puzzle.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | **Tram ride** (2 min): the facility, the Man in the Ugly Sweater behind glass. |
| 2:00 | Lab: put on the suit, push the sample cart; **Resonance Carol-cade**. |
| 4:00 | **Act 1:** a dark, ruined lab; the candy cane; hatcrabs; first scientist follower opens a scanner door. |
| 8:00 | **Act 2:** office complex; Hazard Elf Commandos with squad tactics; long-jump module; HEV chargers. |
| 12:00 | **Act 3:** test silo: the **Big Garland**. Sneak (it reacts to sound), restore fuel, oxygen and power, then fire the rocket engine to burn it. |
| 17:00 | **Finale:** the sample chamber: a Tau-ment boost-jump to the top; the bauble; the surface: daylight… and the Man in the Ugly Sweater closes a door in front of you, then the exit opens. |

## Secrets & challenge
- Floppies: in a microwave in the break room; on a vent ledge only reachable by
  boost-jump; on the Man in the Ugly Sweater's desk (he's not there).
- **Golden Joystick:** "No One Left Behind": bring 3 scientists to the surface.

## Look & sound
- Palette: industrial grey-beige, warning yellow, green alien glow.
- Lightmapped labs with flickering lights and real shadows; physically
  pushable carts and crates.
- Music: sparse electronic tracks starting at scripted moments.

## Multiplayer
- Co-op: each scientist follows the player who "used" him; the silo puzzle
  has three switches to share.
- Versus (bonus): "Crossfire"-style deathmatch in the lab with the Tau-ment.

## Memory
> Somebody's mum came in during the tram ride and asked why we were watching
> a train for two minutes.

## Test hooks
- Scripted sequence system tests (triggers, timelines, interruptibility).
- Follower AI: follows, waits, doesn't block doors (bot pushes through).
- Golden path bot including boost-jump (recorded input).

## Research checklist
- Black Mesa Inbound, Anomalous Materials, Blast Pit, We've Got Hostiles;
  marine AI behaviour; tau jump.
