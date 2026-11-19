# Day 6 — Iron Reindeer
*Homage to **MechWarrior 2: 31st Century Combat** (Activision, 1995)*

## The original
- **Gameplay:** a simulator of giant walking war machines. **Throttle**
  (not "hold W"), **torso twist** independent of leg direction, **jump jets**.
  **Heat management**: every weapon makes heat; overheating shuts the mech
  down (or makes it explode). **Weapon groups** and the **alpha strike**
  (everything at once). **Location damage**: arms, legs and torsos lose armour,
  weapons and limbs separately. **Mechlab**: build your own loadout within
  tonnage, slots and heat sinks. Missions follow **nav points**.
- **Weapons:** lasers (small/medium/large), PPC, autocannons, LRM/SRM missile
  racks, Gauss rifle, machine guns.
- **Look:** flat-shaded and texture-mapped low-poly mechs, a green cockpit
  HUD with wireframe radar, a calm female computer voice ("Heat level critical").
- **Memorable:** the cockpit, the shutdown when you got greedy, legging an
  enemy.

## Homage concept
KRAMPUS.EXE has stolen the **Snow-Titan**, an assault mech from the North
Pole's winter defence. You pilot the **Iron Reindeer**, a 55-ton medium mech
with antlers (antenna arrays). The mission computer **BELLA** speaks in a calm
voice ("Heat level critical. Coolant… festive.").

**Glitch Lieutenant:** the **Snow-Titan**, a 100-ton assault mech.

## Signature mechanics
1. **Mech piloting.** Throttle in steps (reverse to full), legs and torso
   move independently (torso twist ±90°, a leg-direction indicator on the
   HUD), jump jets with a short burn. Footsteps shake the camera (comfort
   option). The cockpit frame stays fixed, which helps with motion sickness.
2. **Heat & weapon groups.** Each weapon adds heat; standing in snow or water
   cools faster; at 100 % the reactor shuts down for 5 s (override possible
   with damage). Weapon groups 1–3 and the alpha strike.
3. **Location damage & Mechlab.** Your and enemy mechs have six damage
   zones; an arm with a weapon can be shot off. Before the mission (and at the
   midpoint repair bay) the **Mechlab** lets you trade weapons, armour and heat
   sinks within 55 tons.

## Arsenal
| Loaner | Notes |
|---|---|
| **Medium lasers** ×2 | no ammo, medium heat |
| **LRM-10 rack** | lock-on, arcing missiles |
| **Particle Present Cannon** | PPC homage: high heat, high damage (trophy) |
| **Autocannon/10** | ammo, low heat |
| Jump jets, heat sinks | mechlab options |

## Enemies
- **Elf-walkers** (light mechs), **hover tanks**, **VTOL sleds**, **turrets**,
  **two medium mechs** (a duel), the **Snow-Titan**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Mission computer briefing with a rotating wireframe of the target. Mechlab (optional "default loadout" button). |
| 1:00 | **Act 1:** Nav Alpha: training on a snow plain; throttle, torso twist, groups; a light-mech patrol. |
| 5:00 | Nav Bravo: defend a convoy; first overheating (shutdown scripted tutorial). |
| 8:00 | **Act 2:** Nav Charlie: a canyon with jump-jet ledges; VTOLs; leg an enemy mech to stop it. Midpoint repair bay and Mechlab. |
| 13:00 | **Act 3:** Nav Delta: a frozen lake at night (cooling bonus, but the ice cracks under heavy mechs); duel with two medium mechs. |
| 16:00 | **Finale:** the Snow-Titan at the dropship; shoot off its arms; the bauble falls out of its torso when it explodes. |

## Secrets & challenge
- Floppies: a hidden cave reachable only with jump jets; under the frozen
  lake (fall through on purpose with a heavy loadout, then walk out); on top
  of the dropship.
- **Golden Joystick:** "Cool Head": finish without a single shutdown.

## Look & sound
- Palette: grey-white snowfields, military olive, HUD phosphor green, laser red.
- Low-poly flat-shaded mechs, long view distance with fog, dynamic
  muzzle/laser lights and shadows of mechs on the snow.
- Music: dark, driving synth-orchestral.

## Multiplayer
- Co-op: a "lance" of up to 4 mechs, each with its own loadout.
- Versus (bonus): mech arena on the frozen lake.

## Memory
> We discovered you could shut down your own mech on purpose to hide from
> radar — and spent an hour doing exactly that.

## Test hooks
- Heat model unit tests (gain, dissipation, shutdown/override).
- Location-damage tests: armour per zone, weapon loss with its arm.
- Mechlab validation (tonnage, slots).

## Research checklist
- First missions of both campaigns; heat behaviour; cockpit HUD layout
  (draw our own).
