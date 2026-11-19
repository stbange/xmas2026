# Day 17 — Knight of the Twelve Nights
*Homage to **Star Wars Jedi Knight: Dark Forces II** (LucasArts, 1997)*

> Legal note: this level must not use any names, sounds, designs or symbols
> from the Star Wars franchise. The homage is in the mechanics: an energy
> blade, mystical powers, a light-or-dark path, and duels.

## The original
- **Gameplay:** an FPS that switches to third person with the energy sword.
  Blaster bolts can be **deflected** with the blade. **Force powers** gained
  through the game: jump, speed, pull, sight, persuasion, healing, push,
  grip, lightning, blinding, destruction… Your choices (killing civilians,
  choosing dark powers) take you down the **light or dark path**, with
  different endings. Seven dark warriors as boss **duels**. Big, vertical
  levels and a legendary sequence on a **falling ship** that tilts as it crashes.
- **Look:** Sith engine; colourful dynamic light from the blade; **live-action
  FMV** cut scenes.
- **Memorable:** the first duel, force jumping across huge spaces, the
  falling ship.

## Homage concept
The **Order of the Twelve Nights** once guarded the spirit of Christmas.
KRAMPUS.EXE has corrupted three of its knights, the **Naughty Knights**. You
find the last **candlesaber** (a blade of light in a candy-cane hilt) and
learn to use the **Spirit**. And from now on, every choice counts: **Nice or
Naughty**.

**Glitch Lieutenants:** three duels with the Naughty Knights: **Sleet**
(fast), **Brimstone** (powers), and their leader **Lord Rime**.

## Signature mechanics
1. **Candlesaber & deflection.** A first-person sword (optional third person
   while it's drawn) with directional swings. Hold *Block*, and incoming bolts
   are deflected; a well-timed block sends them back at the shooter.
2. **Spirit powers.** Spirit-Jump (huge jumps), Spirit-Speed, Pull (grab
   weapons from enemies' hands), Push (knock them off ledges). In act 2 you
   choose one more power: **Nice** (*Heal* or *Persuade*: enemies ignore you)
   or **Naughty** (*Grip* or *Frost Lightning*).
3. **Nice/Naughty alignment.** A meter on the NOEL-Pad. Sparing a surrendering
   enemy, protecting civilians, choosing Nice powers → Nice. Killing those who
   surrender, harming civilians, Naughty powers → Naughty. It persists for the
   rest of December and decides the endings available on day 24.

## Arsenal
| Loaner | Notes |
|---|---|
| **Candlesaber** | trophy |
| **Bolt pistol** | a homage to the hero's trusty sidearm; original design |
| **Thermal snowball** | a grenade |
| Spirit powers | MP-like spirit bar |

## Enemies
- **Tin troopers** (armoured soldiers with blasters, bad aim),
  **Bouncer trolls** with axes, **assassin clockwork elves**,
  the three **Naughty Knights**.

## Level flow (≈20 min)
| Time | Beat |
|---|---|
| 0:00 | Briefing: a grainy "FMV" with the old master (in-engine, filtered). |
| 0:30 | **Act 1:** the smugglers' city in the clouds: blaster combat; find the candlesaber; deflection tutorial. |
| 5:00 | Duel 1: **Sleet** (fast) on a narrow bridge. Spirit-Jump and Push learned. |
| 8:00 | **Act 2:** a market of civilians: alignment choices (a merchant surrenders; civilians in the crossfire). Choose a Nice or Naughty power. Duel 2: **Brimstone**. |
| 13:00 | **Act 3:** **the falling sleigh-ship**: a cargo ship crashes slowly; gravity tilts (the whole level rotates); push through as it falls. |
| 17:00 | **Finale:** duel with **Lord Rime** at the edge of a reactor shaft; he offers you to join KRAMPUS.EXE (a choice: refuse = Nice, accept, then betray = Naughty). Bauble. |

## Secrets & challenge
- Floppies: only reachable with Spirit-Jump; behind a wall that Push breaks;
  in a falling-ship cabin that only opens at a certain tilt.
- **Golden Joystick:** "Pure Heart": finish with maximum Nice (no civilian hurt,
  no surrendering enemy killed).

## Look & sound
- Palette: sky blue and orange sunset clouds, metal grey, blade colours.
- The candlesaber is a dynamic coloured light source with shadows; the
  falling ship rotates the whole level (gravity vector change).
- Music: grand orchestral, our own themes.

## Multiplayer
- Co-op: each player's alignment is tracked separately.
- Versus (bonus): "Duel" — candlesaber 1-vs-1 on a bridge.

## Memory
> We argued all night about whether Christmas is a Nice or a Naughty holiday.
> The vote was a tie.

## Test hooks
- Deflection timing windows (unit), alignment event accounting (unit).
- Gravity rotation: the player controller and physics stay stable under a
  rotating gravity vector.
- Persisted alignment survives save/load and network sessions.

## Research checklist
- Levels 1–3, the "falling ship" level, first duels; force power list
  (rename and redesign all of them).
