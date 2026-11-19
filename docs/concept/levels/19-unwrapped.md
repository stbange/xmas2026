# Day 19 — Unwrapped
*Homage to **Unreal** (Epic MegaGames / Digital Extremes, 1998)*

## The original
- **Gameplay:** a single-player FPS that starts as a prisoner on a **crashed
  prison ship**, escaping onto the alien planet **Na Pali**. The peaceful,
  four-armed natives (Nali) sometimes show you secrets. The **Translator**
  shows messages and logs. The fast, **dodging** Skaarj warriors. Weapons
  with clever **alt-fires**: the upgradable **Dispersion Pistol**, the
  **ASMD shock rifle** (shoot the energy ball with the beam for a combo
  explosion), the **Eightball** (load up to six rockets), the **Flak
  cannon**, Razorjack, GES bio rifle, minigun, sniper rifle. Items:
  **flashlight**, **flares**, **seeds** (plant to grow healing fruit),
  **jump boots**, scuba gear, force field, invisibility.
- **Look:** a technical marvel: **coloured lighting**, **volumetric fog**,
  detail textures, **mirrors**, **procedural fire and water textures**,
  huge outdoor vistas and castles; music in **tracker module** format.
- **Memorable:** stepping out of the ship and seeing the valley; the
  castle flyby intro; a Skaarj appearing in the dark.

## Homage concept
The prison ship **Vortex Jingles** (it took naughty people to their lumps of
coal) crashed on the planet **Na-Holly**. You wake up in your cell when the
power fails. Outside, KRAMPUS.EXE's warrior race, the **Scraaks**, are
enslaving the gentle **Nollys**, four-armed natives who live by Christmas
light. Free them and find the bauble in their temple.

**Glitch Lieutenant:** the **Yeti Titan**, who throws boulders in the temple
courtyard.

## Signature mechanics
1. **Shock combo & upgradable pistol.** The *Shock Rifle*: alt-fire a slow
   energy ball, then hit it with the primary beam for a big explosion.
   The *Dispersion Pistol* is upgraded 4× by energy power-ups found in the level.
2. **Translator & natives.** The Translator shows logs, hints and the
   Nollys' messages. Protect the Nollys (they pray when threatened); grateful
   Nollys show secret doors or give seeds.
3. **Dodge, flares & jump boots.** Double-tap a direction to dodge (the Scraaks
   do it too). Flares light dark caves (dynamic light). Seeds grow into
   healing plants after 10 s. Jump boots for three huge jumps.

## Arsenal
| Loaner | Notes |
|---|---|
| **Dispersion Pistol** | upgradable ×4 |
| **Shock Rifle** | combo (trophy) |
| **Flak Cannon** | shards that bounce; alt: a flak shell |
| **Eightball** | load up to 6 rockets |
| Translator, flashlight, flares, seeds, jump boots | items |

## Enemies
- **Scraaks** (fast, dodge, claws + arm-cannons), **Gruntlers** (slow brutes
  with rockets), **flying Mantas**, **Swamp lurkers**, the **Yeti Titan**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | **Castle flyby** title intro (the camera flies over the castle; skippable), then you wake in a cell. |
| 1:00 | **Act 1:** the crashed prison ship in the dark: flashlight, a Scraak attack in a corridor (classic scare), the Dispersion Pistol. |
| 5:00 | **The vista:** step outside to a huge valley with waterfalls, two moons, a castle in the distance. (Wow moment.) |
| 6:00 | **Act 2:** the Nolly village: protect villagers; seeds; translator logs; Shock Rifle. |
| 11:00 | **Act 3:** dark caves with flares; jump boots up a waterfall; the castle. |
| 15:00 | **Finale:** temple courtyard: the Yeti Titan; inside, the bauble on the altar; Nollys pray together. |

## Secrets & challenge
- Floppies: a Nolly shows you one; one behind a waterfall; one on the
  highest tower of the castle (jump boots).
- **Golden Joystick:** "Guardian": no Nolly dies.

## Look & sound
- Palette: lush greens, purple dusk, gold torchlight.
- The renderer's showcase: coloured **volumetric fog**, **procedural fire
  and water textures**, detail textures, **mirror** floors in the temple,
  coloured dynamic lights and long shadows over a huge vista (LOD and fog).
- Music: **tracker module** music, melancholic and grand.

## Multiplayer
- Co-op: 1–4. Versus (bonus): an arena deathmatch in the temple
  ("Unwrapped Tournament").

## Memory
> The first time we saw the valley, we stopped fighting for five full
> minutes just to look at it.

## Test hooks
- Shock combo: collision of the beam with the projectile (unit + game test).
- Seeds: growth timer, healing; translator log state.
- Render tests: fog and procedural textures against golden images.

## Research checklist
- Vortex Rikers, Nyleve's Falls, the first castle levels; weapon alt-fires;
  dodge timing.
