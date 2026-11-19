# Day 5 — Tides of Eggnog
*Homage to **Warcraft II: Tides of Darkness** (Blizzard, 1995)*

## The original
- **Gameplay:** fantasy RTS, humans versus orcs. Three resources: **gold**,
  **lumber** and **oil** (for ships). Workers (peasants/peons) with famously
  silly lines when you click them too often. Farms raise the unit cap. Guard
  towers. **Naval warfare** (destroyers, battleships, submarines, oil
  platforms), air units (gryphon riders, dragons, zeppelins), the **fog of
  war** and black "shroud". Spells: **polymorph** (into a sheep),
  **blizzard**, **bloodlust**, exorcism, heal, death and decay.
- **Enemies:** grunts, troll axe-throwers, two-headed ogres and ogre-mages,
  death knights, dragons.
- **Look:** gorgeous hand-painted 2D pixel art, including a **winter tileset**.
- **Memorable:** the worker voice lines, clicking the sheep until it explodes,
  the ships.

## Homage concept
The elf village of **Winterhold** is besieged by the **Grumble Horde**, a
gang of grumpy orcs and trolls under KRAMPUS.EXE's banner, who want to turn
the village's eggnog lake into oil. You are the **Captain of the Guard**.
Command your elf workers, rebuild the village, cross the frozen sea and
storm the ogre fortress.

**Glitch Lieutenant:** **Grumble & Mumble**, a two-headed ogre-mage who
argues with himself (and casts bloodlust on his troops).

## Signature mechanics
1. **Command workers & build.** Whistle to give orders to elf workers
   (squad module): chop lumber, mine gold. Resources build structures on fixed
   plots: farm (raises your ally cap), barracks (footmen allies), guard
   tower, shipyard. Click (use) a worker too often and they start complaining
   in increasingly silly lines.
2. **Naval crossing.** Steer a warship across the sea (vehicle module), with
   oil from an oil platform you capture. Ship-to-ship combat with a cannon.
3. **Spells & fog of war.** *Polymorph* turns an enemy into a snow-sheep for
   20 s (poke it too much and it explodes, as tradition demands);
   *Blizzard* is an area spell. Unexplored map regions are covered in a
   black volumetric "shroud" that lifts as you explore (and fog of war greys
   out areas you've left).

## Arsenal
| Loaner | Notes |
|---|---|
| **Sword & shield** | first-person melee with a timed block |
| **Crossbow** | ranged, slow, strong |
| **Polymorph staff** | the snow-sheep spell (trophy) |
| **Blizzard scroll** | 3 uses |
| Gold, lumber, oil | resources |

## Enemies
- **Grumble grunts** (axes), **troll axe-throwers**, **two-headed ogres**,
  **ice dragons** (late), **orc ships** (destroyers), **Grumble & Mumble**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a painted map with a narrator. "Your orders, Captain?" |
| 0:30 | **Act 1:** defend the village from the first raid in melee; give orders to workers; build a farm and barracks. Shroud lifts as you scout. |
| 5:00 | Second raid: guard tower and footmen allies; polymorph tutorial on an ogre. |
| 8:00 | **Act 2:** capture the oil platform; build a shipyard; sail across the eggnog sea; naval battle with orc destroyers. |
| 13:00 | **Act 3:** land at the Horde island; blizzard on the troll camp; dragons attack your ship. |
| 16:00 | **Finale:** storm the ogre fortress with your footmen; Grumble & Mumble boss fight (polymorph one head!). Bauble in the war chest. |

## Secrets & challenge
- Floppies: in a "critter" sheep's pen; on a tiny island in the shroud; in
  the shipwreck.
- **Golden Joystick:** "Exploding Sheep": make a polymorphed sheep explode by
  poking it 10 times.

## Look & sound
- Palette: winter tileset — snow white, pine green, banner blue/red, warm
  timber.
- Chunky, hand-painted voxel buildings; sprite workers and soldiers;
  volumetric shroud.
- Music: rousing medieval march with orchestral brass and bells.

## Multiplayer
- Co-op: one player can command from the town hall's tower overview while
  others fight; separate ships in the naval section.
- Versus (bonus): "Peasant Panic" — two teams of workers race to build a keep.

## Memory
> Somebody spent ten minutes clicking a sheep until it exploded, and the
> whole room cheered louder than for any victory.

## Test hooks
- Squad orders (gather/build/attack) unit tests; worker pathfinding.
- Shroud/fog grid reveal tests.
- Bot run: builds farm/barracks/shipyard and reaches the fortress.

## Research checklist
- Winter maps of the human campaign; worker lines (write our own); spell
  timings.
