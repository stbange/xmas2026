# 03 — Core Gameplay

The core stays the same in every level. Levels add two or three signature
mechanics, a few loaner weapons and items, and their own enemies.

## Verbs (all levels)
| Verb | Keyboard/mouse | Gamepad | Touch |
|---|---|---|---|
| Move / look | WASD + mouse | left / right stick | left floating stick / swipe right half |
| Jump | Space | A | Jump button |
| Crouch | Ctrl / C | B (hold) | Crouch button |
| Sprint | Shift | L3 | double-tap stick forward |
| Fire / alt-fire | LMB / RMB | RT / LT | Fire button (with optional auto-fire) / hold |
| Use / interact | E | X | context button (appears when usable) |
| Melee | V / F | RB | swipe down on fire button |
| Weapon select | 1–9, wheel | Y (wheel) / d-pad | weapon swipe left/right |
| Level ability 1/2 | Q / R | LB / d-pad up | contextual buttons |
| Map / objectives | Tab | View | map button |
| Pause / menu | Esc | Menu | ☰ |

Levels can remap the context buttons (e.g. jetpack in day 9, heat-vent in day
6) through their **input schema**, so touch and XR always show the right
buttons.

## Movement
- Fast, classic FPS movement: ground speed ~10 m/s, air control, step-up for
  stairs, no fall damage (except in day 4).
- Advanced moves (strafe-jumping, rocket-jumping, ski, dodge, 6DOF flight) are
  **level mechanics**, enabled only where their homage needs them.
- Optional: auto-run, toggle crouch, head-bob off, FOV 60–110.

## The base kit (always with you)
| Item | Function |
|---|---|
| **Candy Cane** (melee) | Bash, open vents and crates, pull levers from a distance with the hook. |
| **Snowball Blaster** (sidearm) | Infinite ammo; tap for fast snowballs, hold to charge a heavy snowball that stuns. |
| **NOEL-Pad** (wrist computer) | Map, objectives, hints, the bauble tracker. In XR it's a real wrist menu. |

Each level then **lends** you its own arsenal. Loaner weapons and items exist
only in their level. This keeps every level balanced on its own. After a
level is finished, one loaner becomes a **trophy** on the hall's trophy wall,
for use in the Snowball Arena, Remix Mode and the finale (day 24 lets you pick
six trophies).

## Health & resources
- **Health** 100 (max 200 with overcharge pickups that slowly drain).
- **Armour** "Tinsel Armour" 0–100, absorbs 50 % of damage.
- Pickups: cocoa mug (+10), gingerbread (+25), Christmas pudding (+50), each
  level re-skins them in its homage style (Duke-style medkit, Diablo potion,
  HEV charger …).
- Levels can add their own resource bars (heat, mana, energy, oxygen, stamina,
  time units, credits). There are **at most two extra bars** per level.

## Death, checkpoints, saving
- Checkpoints at every section of a level (≈ every 3–5 minutes); some levels
  use homage checkpoints (day 4's save crystals, day 20's resurrection bays).
- Death: restart from the last checkpoint with the health/ammo you had there.
- Quick-save anywhere on desktop (F5/F9) on Normal or easier; not in versus.
- Progress is saved automatically: completed levels, collectibles, best times,
  alignment, trophies.
- In co-op, a downed player can be **revived** by a teammate within 20 s
  (hold Use); otherwise they respawn at the last checkpoint after 10 s.

## Collectibles per level
| Collectible | Count | Reward |
|---|---|---|
| **Memory Bauble** | 1 | Required to finish the level; hangs on the tree; plays a memory |
| **Gingerbread Floppies** (secrets) | 3 | Unlock jukebox tracks, concept art, the friends' 2001 "screenshots" |
| **Golden Joystick** (challenge) | 1 | Level-specific challenge (par time, no damage, pacifist, ...) |

## Difficulty
| Name | Damage taken | Enemies | Notes |
|---|---|---|---|
| **Pizza** | 50 % | fewer, slower | aim assist, extra checkpoints |
| **Cola** (default) | 100 % | normal | |
| **Coffee at 4 AM** | 150 % | more, smarter | fewer pickups |
| **Nightmare LAN** | 200 % | respawn | unlocked after day 24, no checkpoints mid-section |

Puzzle difficulty doesn't change, but NOEL hints become available sooner on
Pizza.

## The level template (≈20 minutes)
Every level follows the same rhythm, so production can be planned and
tested:

| Phase | Time | Content |
|---|---|---|
| **Briefing** | 0:30 | Briefing in the style of the original; NOEL summary |
| **Act 1 — Learn** | 4–5 min | Teach signature mechanic A in safety, then under pressure |
| **Act 2 — Combine** | 5–6 min | Introduce mechanic B; first combination with A |
| **Act 3 — Twist** | 5–6 min | Mechanic C or a twist on A+B; the level's "wow" set piece |
| **Finale** | 3–4 min | The Glitch Lieutenant (boss or big challenge), bauble |
| **Exit** | 0:30 | Memory vignette, results screen, back to the hall |

Target times are measured by playtests on *Cola* by someone who knows FPS
games but not the level; an automated bot run measures the lower bound (see
[quality](08-quality-and-testing.md)). A level with a playtest median between
15 and 25 minutes is "on target".

## Mechanic modules
Level mechanics are built as reusable, tested C modules so they can be
combined and reused (e.g. day 20's cyberspace re-uses day 11's 6DOF flight).

| Module | Used by days |
|---|---|
| `flight6dof` (free 6DOF flight) | 11, 20 |
| `vehicle` (carpet, mech, ship, sled) | 2, 5, 6, 9 |
| `heightfield` (deformable terrain) | 2, 9, 21, 23 |
| `voxel_destruct` (destructible voxel objects) | 10, 15, 23, 24 |
| `squad` (AI followers and orders) | 3, 5, 8, 10, 15, 21, 23, 24 |
| `economy` (resources, build plots) | 2, 3, 5, 7, 21, 23 |
| `tactical_time` (pause/time-units, bullet-time) | 15, 20 |
| `status_bars` (heat, energy, mana, oxygen, stamina, shield) | 2, 4, 6, 9, 14, 18, 20 |
| `spells` (area effects, conversion, polymorph) | 2, 5, 14, 17, 18, 21 |
| `round_rules` (rounds, objectives, buy phase, CTF) | 7, 9 |
| `procgen_dungeon` (seeded dungeon generator) | 12 |
| `loot` (random affixes, inventory grid) | 12, 20 |
| `alignment` (Nice/Naughty) | 17+ (global) |

## Accessibility & comfort
- Full rebinding; toggle/hold for every hold action; aim assist settings.
- Subtitles and visual cues for all important sounds.
- Colour-blind safe objective markers; palette effects can be turned down.
- Motion: FOV slider, head-bob off, camera shake off, "comfort mode" for
  6DOF and vehicle levels (auto-level horizon, vignette).
- Photosensitivity: strobe and flash effects can be disabled.
- Gore: Off / Confetti / Classic.

## Personalisation pack
The levels are a gift to a specific group of friends, so the game can be
personalised without code changes. A folder `content/personal/` (not part of
the public repo by default) can contain:

```
content/personal/
  crew.json            # nicknames, colours, avatar choice, bot personalities
  memories/01.txt      # replaces day 1's memory text
  memories/01.png      # optional 2001 photo, displayed palette-quantised
  tags/<nick>.png      # spray tags (day 7), graffiti, banners
```

- The crew's nicknames are used for NPC allies, bot opponents (day 7, 9),
  XCOM soldiers (day 15), graffiti and the credits.
- Photos are shown only if the pack is present. Because the gh-pages build is
  public, the pack should be loaded from a private URL or a local file
  ("Load memories…" in NOEL's menu) rather than committed.
