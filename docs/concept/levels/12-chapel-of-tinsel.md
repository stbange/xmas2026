# Day 12 — Below the Chapel of Tinsel
*Homage to **Diablo** (Blizzard North, 1996)*

## The original
- **Gameplay:** an isometric action RPG. A small town (**Tristram**) above a
  16-level dungeon (cathedral, catacombs, caves, hell), **randomly generated**
  each game. Click to fight, **loot** with random **prefixes/suffixes**
  (magic = blue, unique = gold), an inventory grid, a **potion belt**,
  **Town Portal** scrolls, identify scrolls. **Shrines** with random, sometimes
  bad effects. Classes: warrior, rogue, sorcerer. The **Butcher** ("Ah, fresh
  meat!"). The town's old storyteller and the blacksmith.
- **Look:** dark, gothic pre-rendered sprites; a **light radius** around the
  hero; everything outside it is black.
- **Memorable:** the guitar of the town's music, the Butcher's line, the loot
  lottery.

## Homage concept
The village of **Kristmas Hollow** has a problem: something lives under the
**Chapel of Tinsel**. The **Lord of the Pit**, a demon corrupted by
KRAMPUS.EXE, has taken the bauble down to the deepest floor. Go down four
floors, get stronger with loot, and come back alive.

**Glitch Lieutenants:** **The Baker** on floor 2 ("Ah, fresh *dough*!"), and
the **Lord of the Pit** on floor 4.

## Signature mechanics
1. **Seeded procedural dungeon.** Four floors (cathedral, catacombs, ice
   caves, "hell" under the tree) generated from room templates. The seed is
   the **date**, so all friends get the same dungeon that day and can compare
   (and share tips). Remix Mode uses random seeds.
2. **Random loot.** Enemies and chests drop items with random affixes
   (common/magic/rare/unique colours), an inventory grid, a 4-slot **potion
   belt** (health, mana, rejuvenation), identify scrolls, gold. Stats affect
   damage, speed, light radius, etc.
3. **Light radius, Town Portal & shrines.** The hero carries a lantern: the
   world is pitch black outside its radius (dynamic light with shadows; items
   can change the radius). **Town Portal** scroll back to the village to
   sell, buy and heal, and the portal back stays open. **Shrines** with random
   effects ("Mysterious", "Hidden", "Weird"…).

## Arsenal
| Loaner | Notes |
|---|---|
| Swords, axes, bows, staves | random loot with affixes |
| **Lantern of Kristmas** | your light radius (trophy: a unique lantern) |
| Potion belt, Town Portal scrolls | consumables |
| Spell books: firebolt, heal, town portal | |

## Enemies
- **Fallen elves** (small, run away when their leader dies), **skeletons**,
  **gingerbread golems**, **snow witches** (teleport), the **Baker**, the
  **Lord of the Pit** (fire, summons).

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: the elder **Candlecane** tells the story by the fire (guitar music). |
| 1:00 | **Kristmas Hollow:** blacksmith, healer, starter gear. |
| 2:00 | **Floor 1 (cathedral):** loot tutorial; the first shrine; light radius. |
| 6:00 | **Floor 2 (catacombs):** "Ah, fresh dough!" — the Baker behind a door. Town Portal back to heal. |
| 11:00 | **Floor 3 (ice caves):** better loot; a unique item; rare monsters with affixes. |
| 15:00 | **Floor 4 (the pit):** the Lord of the Pit; bauble; Town Portal home. |

## Secrets & challenge
- Floppies: three randomly placed "hidden" rooms (one per floor 1–3), found
  via a **Hidden Shrine** or a cracked wall.
- **Golden Joystick:** "Ironman": no Town Portal used.

## Look & sound
- Palette: dark, gothic browns and reds; candle orange; ice blue on floor 3.
- First person with a strong **light radius** look; pre-rendered-style
  sprites lit by dynamic light; deep shadows.
- Music: slow, acoustic guitar for the village; dark ambient in the dungeon.

## Multiplayer
- Co-op: up to 4 heroes; loot drops per player (no stealing).
- The daily seed lets friends race each other on the same dungeon (leaderboard).

## Memory
> Four people crowded around one monitor, waiting for the Butcher's door to
> open. The person with the mouse refused to open it for five minutes.

## Test hooks
- Procedural generator: for 10 000 seeds, every floor is connected, the stairs
  are reachable, and generation is deterministic per seed.
- Loot tables: affix rolls stay within bounds; drop rates over 1M rolls.
- Inventory grid unit tests (fit, swap, overflow).

## Research checklist
- Tristram, cathedral and catacombs layouts and pacing; affix lists for
  inspiration (write our own); shrine effects.
