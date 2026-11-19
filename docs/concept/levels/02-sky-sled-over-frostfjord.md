# Day 2 — Sky Sled over Frostfjord
*Homage to **Magic Carpet 2: The Netherworlds** (Bullfrog, 1995)*

## The original
- **Gameplay:** a wizard on a flying carpet over 3D landscapes. You shoot
  **mana balls** to claim them in your colour; your **castle's balloon**
  collects them, and the castle grows with mana. Rival wizards steal mana and
  attack your castle. Spells: fireball, lightning, **raise/lower land**,
  earthquake, volcano, meteor, **possession** (claim mana), rebound, heal,
  castle. Monsters: wyverns, griffins, giant worms, crabs, bees, genies,
  skeletons. MC2 added **night levels** and **underground caves**.
- **Look:** a smooth heightfield landscape rendered in real time, reflective
  water, lots of particles; it even had a stereoscopic 3D mode.
- **Memorable:** the terrain changing as you cast spells, and the castle
  growing in tiers.

## Homage concept
Santa's sleigh is grounded (KRAMPUS.EXE ate the reindeer's flight memory), so
NOEL gives you a **sky sled**, a flying carpet with a sled runner. Over the
fjords of Frostfjord, the frost wizard **Count Frostbeard**, KRAMPUS.EXE's
lieutenant, is draining the region's *glimmer* (mana). Grow your gingerbread
castle, take back the glimmer, and blow up his ice castle.

**Glitch Lieutenant:** Count Frostbeard on a wyvern, who has his own castle and balloons.

## Signature mechanics
1. **Free flight.** The sled flies with mouse/stick steering; it lands and takes off;
   you can step off and walk in caves. Comfort mode: auto-level, vignette.
2. **Glimmer & castle.** Shoot glimmer orbs to claim them; your **gift
   balloon** flies them home. The castle grows in four tiers (each adds a
   turret and raises its health); the rival's balloons steal unclaimed and
   enemy-coloured orbs, so you shoot them down.
3. **Terraforming.** *Raise Land* / *Lower Land* spells deform the heightfield
   in real time: raise a dam to stop a lava flow, lower a ridge to open a
   cave, build a ramp for walking allies.

## Arsenal
| Loaner | Notes |
|---|---|
| **Fireball** | primary spell, charges up to a triple fireball |
| **Lightning** | chain hits flying enemies |
| **Possession** | claims enemy-coloured glimmer |
| **Raise/Lower Land** | terraforming (trophy: "Snow-shaper") |
| **Castle** | place or grow your castle |

Spells cost glimmer (mana); the castle stores it.

## Enemies
- **Frost wyverns** (fly, breathe ice), **snow crabs** (giant, beach),
  **ice worms** (burrow through the heightfield), **glacier bees** (swarm).
- Count Frostbeard: a rival wizard AI that builds, steals and duels.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing on a parchment map; NOEL hands over the sled. |
| 0:30 | **Act 1:** flight tutorial through ice arches; shoot glimmer; place the castle on a hill. |
| 5:00 | Snow crabs on the beach; the balloon starts collecting; castle tier 2. |
| 7:00 | **Act 2:** Frostbeard's balloons steal; Lower Land to open the cave entrance; on foot through the **underground cave** (MC2 homage), lit only by spell light. |
| 12:00 | **Act 3:** night falls (dynamic sky, moonlight shadows, spell lights); a volcano threatens your castle: raise a dam. Castle tier 3–4. |
| 16:00 | **Finale:** duel with Count Frostbeard on his wyvern; destroy his castle; the bauble falls out of the ruins. |

## Secrets & challenge
- Floppies: on a pillar in a hidden ice cave; inside a worm's tunnel; on top
  of an iceberg only reachable by raising the sea floor.
- **Golden Joystick:** "Landlord": win with no glimmer ever stolen from your castle.

## Look & sound
- Palette: icy blues, aurora greens and violets, warm gingerbread brown.
- Smooth-shaded heightfield with vertex-coloured snow, planar water
  reflections, many particles; aurora sky; at night, every fireball lights
  the terrain.
- Music: orchestral-synth, airy, with bells.

## Multiplayer
- Co-op: shared castle, each player a wizard with their own colour; up to 4
  sleds.
- Versus (bonus): "Glimmer war" — each player builds a castle; the most
  glimmer after 10 minutes wins.

## Memory
> The night someone flew so long they got motion sickness and had to lie down
> under the table with a bag of crisps.

## Test hooks
- Heightfield deformation unit tests (raise/lower clamps, normals, collision
  updates).
- Balloon path-finding always returns to the castle.
- Bot run: claims ≥ X glimmer and reaches castle tier 4 within 18 minutes.

## Research checklist
- MC2 night level and cave level longplay; spell list and castle tiers;
  balloon behaviour.
