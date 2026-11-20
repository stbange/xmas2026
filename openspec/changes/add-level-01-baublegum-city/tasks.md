# Tasks

## 1. Framework

- [x] 1.1 Level definition, manifest validation, registry with test-course placeholders
- [x] 1.2 Simulation wiring: names, relays, movers, usables, light groups, hurt volumes, shake, events
- [x] 1.3 Sandboxed Lua script host and `tb` API; build-time embedding of level scripts
- [x] 1.4 Golden-path bot and headless golden-path runner
- [x] 1.5 App integration: load levels, run scripts, NOEL lines and objectives in the HUD, `--level`/`--bot` (`?level=`/`?bot=`)
- [x] 1.6 Renderer: level colours, props, light groups, enemies, usables, movers, hologram, shake

## 2. Day 1: Baublegum City

- [x] 2.1 Enemies: Hogwash Trooper, Elf-Lizard, Brain-bauble, Bargain-Bin Battlelord
- [x] 2.2 Weapons and items: Mighty Boot, Pump Shotgun, Pipe Baubles, Gift-Shrinker, Jetpack, Holo-Santa; shrink zones and stomping
- [x] 2.3 Layout: roof, stairwell, concourse, cinema, toy store, vent, atrium, parking deck, exit
- [x] 2.4 Script: story beats, arcade floppy, earthquake, boss, Interior Decorator
- [x] 2.5 Golden path on all difficulties (Nightmare: known winning seed)
- [ ] 2.6 Sprites/voxel models for enemies and props (boxes for now)
- [ ] 2.7 Grow the level towards the 15–25 minute target (Bot lower bound)

## 3. Quality

- [x] 3.1 Unit tests for world, AI, script, level, bot, HUD and flow; coverage gate
- [x] 3.2 Web smoke test starts day 1 directly on the playtest build; release ignores `?level=`
