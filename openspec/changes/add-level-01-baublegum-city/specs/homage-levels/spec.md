# Spec Delta

## Purpose
The 24 individual homage levels. Each level change adds its own requirements here.

## ADDED Requirements

### Requirement: Day 1 — Baublegum City
Day 1 SHALL be "Baublegum City", a night-time mall overrun by alien boars, played from the roof down to the parking deck. Its signature mechanics SHALL be the interactive world, shrink-and-stomp, and the jetpack with the Holo-Santa decoy. It SHALL lend the Mighty Boot, Pump Shotgun, Pipe Baubles, Gift-Shrinker (the trophy), Jetpack and Holo-Santa. It SHALL end with the Bargain-Bin Battlelord guarding the Memory Bauble.

#### Scenario: Baublegum City golden path
- **WHEN** the golden-path bot plays day 1 on Cola difficulty
- **THEN** it collects all three floppies and the Memory Bauble and reaches the exit, and the same seed always gives the same run

#### Scenario: Interior Decorator
- **WHEN** the player uses one of every kind of usable object in the level (breaker switch, toilet, vending machine, arcade cabinet, projector button)
- **THEN** the level's Golden Joystick challenge is won

#### Scenario: Hogwash Trooper ambush
- **WHEN** a Hogwash Trooper sees the player
- **THEN** it closes in and fires its shotgun, with fewer pellets landing the further away the player is

### Requirement: Day 1 gear
The Gift-Shrinker SHALL shrink ordinary enemies for six seconds, during which they flee and die when stomped. Pipe Baubles SHALL be thrown, bounce, come to rest and explode on the alternative fire button or after 20 seconds. The Jetpack SHALL thrust upwards on its fuel and give real air control while thrusting. The Holo-Santa SHALL project a decoy for 8 seconds, with a 15-second recharge.

#### Scenario: Shrink and stomp
- **WHEN** the player walks into a shrunken enemy
- **THEN** the enemy is stomped flat and its death is reported to the level script

#### Scenario: Boss shrugs off the shrinker
- **WHEN** the Gift-Shrinker hits the Bargain-Bin Battlelord
- **THEN** the Battlelord does not shrink

#### Scenario: Pipe bauble remote detonation
- **WHEN** a thrown pipe bauble has come to rest and the player presses alternative fire
- **THEN** it explodes with splash damage and a camera shake, and it never explodes on contact

#### Scenario: Shrink zone
- **WHEN** the player stands in the Shrink-O-Matic zone
- **THEN** they become mouse-sized and fit through the vent, and grow back only once there is room

#### Scenario: Jetpack
- **WHEN** the player holds the jetpack button with fuel left
- **THEN** they rise, fuel drains, and they can steer in the air while thrusting

#### Scenario: Holo-Santa decoy
- **WHEN** the player activates the Holo-Santa
- **THEN** a hologram appears in front of them for 8 seconds, and a second one is only available after the 15-second recharge

#### Scenario: Holo-Santa distraction
- **WHEN** an enemy can see both the player and the hologram
- **THEN** it targets the hologram until the hologram is destroyed or fades, then returns to the player
