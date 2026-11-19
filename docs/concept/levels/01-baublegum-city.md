# Day 1 — Baublegum City
*Homage to **Duke Nukem 3D** (3D Realms, 1996)*

## The original
- **Gameplay:** fast, run-and-gun FPS in a "2.5D" Build-engine world (sectors
  with sloped floors, room-over-room tricks). Its claim to fame was
  **interactivity**: working toilets, mirrors, light switches, pool tables,
  arcade machines, strippers (not here), breakable glass, and destroyable
  buildings after earthquakes. The hero cracks one-liners all the time.
- **Weapons/items:** Mighty Foot kick, pistol, shotgun, ripper chaingun,
  RPG, **pipe bombs** (remote detonated), **Shrinker** (shrink, then stomp),
  Expander, Devastator, laser trip mines, Freezethrower. Inventory: **jetpack**,
  **Holoduke** (decoy hologram), steroids, night-vision goggles, portable medkit, scuba gear.
- **Enemies:** pig-faced alien police, teleporting lizard troopers, flying
  octopus brains, the Battlelord boss with a chaingun and mortar.
- **Look:** 256-colour sprites (8 directions), pop-culture-filled urban Los Angeles,
  neon, rooftops, a cinema, an arcade.

## Homage concept
On Christmas Eve 2001 a sleigh crashes onto the roof of a neon-lit shopping
district, **Mistletoe Mall**. Alien boars in Santa hats (the **Hogwash
Security**) have taken over and are wrapping shoppers into gift packages.
KRAMPUS.EXE grins from a giant video wall: `<KRAMPUS.EXE> welcome 2 my mall n00b`.
Your avatar has a one-liner for everything (toggle in the options).

**Glitch Lieutenant:** the **Bargain-Bin Battlelord**, a four-armed alien
Santa with a minigun and a present-mortar, on the top of the parking deck.

## Signature mechanics
1. **Everything is interactive.** Switches really switch dynamic lights,
   mirrors reflect the player (real planar reflections), arcade machines play
   a 30-second mini-game, toilets heal 1 HP, vending machines give cola for
   coins, glass breaks. Interaction rewards secrets and one-liners.
2. **Shrink & stomp.** The *Gift-Shrinker* shrinks enemies for 6 s so you can
   stomp them. A trap shrinks *you* for a vent section through the toy store
   at mouse size (scale puzzle; enemies become giants).
3. **Jetpack & Holo-decoy.** A fuel-limited jetpack for the rooftop and
   parking-deck sections; the *Holo-Santa* projects a decoy of you that draws
   enemy fire for 8 s.

## Arsenal
| Loaner | Notes |
|---|---|
| **Mighty Boot** | quick kick, works while holding another weapon |
| **Pipe Baubles** | throw, then remote-detonate; bounce on slopes |
| **Gift-Shrinker** | shrink ray (trophy) |
| Pump shotgun | the classic |
| Jetpack, Holo-Santa, night-vision goggles | inventory items |

## Enemies
- **Hogwash Troopers** — alien boar mall cops with shotguns; sprite, 8 angles.
- **Teleporting Elf-Lizards** — warp in with a flash, shoot plasma.
- **Brain-baubles** — floating octopus ornaments that throw psychic snowballs.
- **Bargain-Bin Battlelord** — boss.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: news-ticker style. Crash on the roof; kick a vent open (tutorial for kick/jump). |
| 1:00 | **Act 1:** rooftop and stairwell into the mall; the first mirror in a restroom; light switches; first Hogwash Troopers. |
| 5:00 | Cinema with a giant screen showing a low-res "movie"; arcade with a playable mini-game (secret code). |
| 8:00 | **Act 2:** toy store. Shrink trap → mouse-sized vent run over toy trains, then the Gift-Shrinker is found and you shrink enemies back. |
| 13:00 | **Act 3:** an earthquake (scripted) tears open the main hall; jetpack across the collapsing atrium; Holo-Santa to get past a turret nest. |
| 17:00 | **Finale:** parking deck. Battlelord fight; pipe baubles on the mortar ramps. |
| 19:30 | Bauble in the boss's sack. KRAMPUS.EXE grabs the tree star and flees: the story begins. |

## Secrets & challenge
- Floppies: behind the cinema screen; in the arcade high-score table (enter
  the code from a poster); jetpack to the top of the neon sign.
- **Golden Joystick:** "Interior Decorator": interact with 20 different object
  types in one run.

## Look & sound
- Palette: neon pinks, cyans, sodium orange, wet asphalt.
- Build-engine feel: sector-like rooms, sloped ceilings, sprite enemies,
  flat-lit areas with strong coloured neon dynamic lights; mirrors and
  reflective floors.
- Music: chunky MIDI-style rock riff with sleigh bells.

## Multiplayer
- Co-op 1–4: the shrink vent fits everybody; the Holo-Santa decoy shows the
  thrower's nickname.
- The mall's food court is reused as a small deathmatch map in the Snowball
  Arena rotation.

## Memory
> The first frag of the night, and somebody shouting "Damn, I'm good" across
> the hall, loud enough that the caretaker came to check.

## Test hooks
- Bot can complete every act; shrink vent cannot soft-lock (unshrink
  triggers at both ends).
- Mirror renders the player model (render test with golden image).
- All 20 interaction types fire their events (unit tests on the
  interaction module).

## Research checklist
- E1L1–E1L3 longplay (rooftop, cinema, Red Light District street).
- Shrinker/stomp timing; jetpack fuel; Holoduke duration.
