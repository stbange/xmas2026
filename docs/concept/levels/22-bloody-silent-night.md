# Day 22 — Bloody Silent Night
*Homage to **Blood** (Monolith Productions, 1997)*

## The original
- **Gameplay:** a Build-engine FPS with dark, gleeful horror humour. You are
  **Caleb**, an undead gunslinger back from the grave ("I live… again!"),
  taking revenge on the cult of a dark god. **Cultists** shout in a made-up
  language. Weapons: **pitchfork**, **flare gun** (sets enemies on fire, and
  burning enemies run around screaming), sawed-off shotgun, tommy gun,
  **napalm launcher**, tesla cannon, **aerosol can + lighter** flamethrower,
  **dynamite** (bundle, proximity, remote; **hold to throw further**),
  **voodoo doll**, life leech. The **Guns Akimbo** power-up doubles your
  weapon.
- **Enemies:** cultists, axe zombies, gargoyles, hellhounds, gill beasts,
  spiders, Cheogh, the dark god Tchernobog.
- **Look:** gothic Build-engine levels full of horror-movie references: a
  train, a carnival, a hotel, a cemetery. Sprites with a lot of fire.
- **Memorable:** setting cultists on fire with the flare gun, the one-liners,
  the dynamite throw, the voodoo doll.

## Homage concept
The darkest level. You follow KRAMPUS.EXE into his lair: a haunted carnival
on a frozen lake and a cursed hotel. His cult, the **Silent Nighters**,
chant carols backwards. You come back from the grave every time you die
("I'm home… for the holidays!"). In the hotel's ballroom you find his **sack
of stolen memories** and learn his plan for Christmas Eve.

**Glitch Lieutenant:** **The Great Grumbletrog**, a dark god made of
KRAMPUS.EXE's discarded data, in the carnival's big top.

Gore setting: on *Confetti*, everything bursts into gift wrap and confetti.

## Signature mechanics
1. **Fire.** The **flare gun** sticks flares into enemies and sets them on
   fire; burning enemies run around, spread fire to others, set hay, banners
   and curtains alight. The aerosol + lighter flamethrower. Fire is a dynamic
   light source (lots of them) and a gameplay element (burn webs and doors).
2. **Charged dynamite.** Hold to light and wind up; the longer you hold, the
   further it flies (but don't hold too long). Variants: bundle, proximity,
   remote.
3. **Voodoo doll & Guns Akimbo.** The **Krampus doll**: stab it to hurt the
   enemy you're looking at (or yourself if nobody is there). **Guns Akimbo**
   doubles any weapon for 30 s.

## Arsenal
| Loaner | Notes |
|---|---|
| **Pitchfork** | melee |
| **Flare gun** | fire (trophy) |
| **Sawed-off shotgun** | double barrel |
| **Tommy gun** | |
| **Dynamite** | charged throw |
| **Aerosol + lighter**, **Krampus doll**, **Guns Akimbo** | |

## Enemies
- **Silent Nighters** (cultists with shotguns and tommy guns, chanting carols
  backwards), **axe zombies** (they come back unless burnt or blown up),
  **gargoyles**, **hellhounds with antlers**, **ghostly mimes** (an in-joke),
  **spiders**, the **Great Grumbletrog**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a cemetery at night; you climb out of your grave. "I'm home… for the holidays!" |
| 0:30 | **Act 1:** the haunted train to the carnival: pitchfork, flare gun, cultists in the carriages. |
| 5:00 | **Act 2:** the frozen-lake carnival: a ghost train ride, a house of mirrors (mirrors!), a shooting gallery mini-game, dynamite. |
| 11:00 | **Act 3:** the **Overcooked Hotel**: long corridors, twin ghost elves, a hedge maze in snow, the ballroom with the **sack of memories** (story beat). |
| 16:00 | **Finale:** the big top: the Great Grumbletrog. Guns Akimbo, voodoo doll. The bauble is in the god's heart. |

## Secrets & challenge
- Floppies: in the house of mirrors; behind a shooting gallery target; in room 237
  (of course).
- **Golden Joystick:** "Pyromaniac": set 25 enemies on fire.
- Easter egg: record the cultists' chant and play it backwards (the jukebox
  has a "reverse" button): it's "Jingle Bells".

## Look & sound
- Palette: blacks, blood/wine red, candle yellow, ghostly green.
- Heavy use of shadows and many small dynamic fire lights; Build-engine
  architecture with sloped floors; sprite enemies.
- Music: dark carnival organ and horror strings.

## Multiplayer
- Co-op: 1–4. Versus (bonus): "Bloodbath" deathmatch in the hotel.

## Memory
> At 4 AM someone put a horror film on the projector between matches. Half of
> us slept with the lights on. The other half pretended not to care.

## Test hooks
- Fire propagation (bounded spread, burns out, performance cap on lights).
- Dynamite charge → throw velocity curve; fuse timing.
- Voodoo doll targeting tests; self-damage when no target.

## Research checklist
- E1 (The Way of All Flesh): train, carnival, hotel levels; flare gun and
  dynamite behaviour; cultist language (invent our own).
