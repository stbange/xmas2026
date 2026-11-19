# Day 14 — Song of the Frost Tree
*Homage to **Secret of Mana** (Square, 1993, SNES)*

## The original
- **Gameplay:** an action RPG for up to **three players at once** (with
  the multitap). The **Ring Command menu**: a ring of icons around the
  character that pauses the action. A **stamina gauge** (0–100 %): attacking
  at less than 100 % does little damage; hold the button to **charge** the
  weapon to higher levels. Eight weapon types (sword, spear, bow, axe,
  **whip** (swing over gaps), boomerang, javelin, glove); magic from
  **elemental spirits** (water, earth, wind, fire, dark, moon, light,
  wood). Flying on the white dragon, cannon travel.
- **Enemies:** the cute, bouncing **Rabites**, mushboom mushrooms, and a winter
  area (Ice Country) with a frost giant.
- **Look:** colourful, painterly 16-bit sprites, Mode 7 for flight.
- **Memorable:** three friends on one SNES, the Ring menu, the music.

## Homage concept
In the middle of a frozen forest stands the **Frost Tree**, which sings the
song that keeps winter gentle. KRAMPUS.EXE has corrupted it and made it sing
off-key: the forest is turning into a hostile blizzard. With two companions
(AI or friends), climb to the tree's crown and restore the song.

**Glitch Lieutenant:** the **Frost Colossus**, a giant possessed by the tree's
corrupted song.

## Signature mechanics
1. **Ring menu.** Press *Ring* and a ring of items/spells/weapons surrounds you
   while the game pauses; rotate and choose. In XR it's a real ring around the
   wrist. It replaces the normal weapon wheel in this level.
2. **Stamina charge & whip-grapple.** The stamina gauge refills after every
   swing; at 100 % you hit hard; hold to charge up to level 2 for a special
   attack. The *Holly Whip* swings you across gaps on posts.
3. **Spirit magic & 3-player co-op.** Two companion characters (sprites) with
   their own weapons and spells, controlled by AI or split-screen friends
   (the "multitap" homage: this is the best split-screen level). Spirits of
   snow, wind and fire give spells like *Freeze*, *Gust* and *Warm Hearth* (heal).

## Arsenal
| Loaner | Notes |
|---|---|
| **Sword of the Solstice** | charge levels 1–2 |
| **Holly Whip** | grapple on posts (trophy) |
| **Boomerang** | hits and returns |
| Spirit magic: Freeze, Gust, Warm Hearth, Flame | spells (MP) |
| Candy (heal), Chocolate (more heal), Walnut (MP) | items |

## Enemies
- **Snowbites** (bouncing snow bunnies with fangs), **mushgnomes**,
  **frost wolves**, **wandering pines**, the **Frost Colossus**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a storybook page, a watercolour illustration and text. |
| 0:30 | **Act 1:** a snowy forest path; companions join; stamina and charge tutorial; Snowbites. |
| 5:00 | A village of snow-folk (shop, inn); Ring menu tutorial. |
| 7:00 | **Act 2:** the whip; frozen river crossings; the first spirit (Wind). |
| 12:00 | **Act 3:** a flight on the **snow dragon** to the tree (a Mode-7-style flight sequence, short and on rails), then climb the tree's branches. |
| 16:00 | **Finale:** the Frost Colossus in the crown; combo of charge attacks and spells; the tree sings again; the bauble falls like a fruit. |

## Secrets & challenge
- Floppies: inside a snowman you have to melt; behind a whip post across a
  chasm; in the inn's attic.
- **Golden Joystick:** "Full Charge": defeat the Colossus using only level-2
  charge attacks.

## Look & sound
- Palette: bright, pastel winter; soft blue snow, warm lantern pinks.
- Clean pixel-art sprites (billboards) in a 3D world; a flat, painterly
  look with soft dynamic light; Mode-7-like flight over a flat world map.
- Music: lush, melodic, a little melancholic.

## Multiplayer
- **Split screen with 3 players** is the showcase. Online co-op 2–4.

## Memory
> Three of us on one tiny TV, years before this LAN, arguing about who has to
> be the sprite girl. This night we did it again on a projector.

## Test hooks
- Stamina gauge and charge-level unit tests.
- Companion AI: follows, doesn't fall off ledges, uses heals.
- Ring menu input tests on all input types (keyboard, pad, touch, XR).

## Research checklist
- Ice Country and Santa's area; Ring menu behaviour; charge timings.
