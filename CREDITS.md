# Credits

## Project

- Concept, direction: Stephan Bange
- Code: MIT License (see `LICENSE`)
- Art & content: CC BY 4.0 (see `LICENSE-ART`)

## How to attribute (CC BY 4.0)

> "Advent of LAN" art by Stephan Bange and contributors, licensed under
> CC BY 4.0 — https://github.com/stbange/xmas2026

Every asset with its own author must be listed in `assets/ATTRIBUTION.md`
with: file path, author, license, source URL, and any changes made. CI checks
that every file under `assets/` has an entry there (see
`docs/concept/08-quality-and-testing.md`).

## Third-party code (planned; all permissive, compatible with MIT)

| Library | Use | License |
|---|---|---|
| SDL3 | window, input, audio, platform | zlib |
| cglm | math | MIT |
| cgltf | glTF model loading | MIT |
| stb_image / stb_truetype / stb_vorbis | images, fonts, audio | MIT / public domain |
| Lua 5.4 | level choreography scripts | MIT |
| cJSON | config, save files | MIT |
| ENet | desktop LAN UDP transport | MIT |
| Unity (ThrowTheSwitch) | unit-test framework | MIT |
| Emscripten | web toolchain | MIT / NCSA |

Tools that are only used to make content (TrenchBroom, Blender, Goxel,
LibreSprite, a tracker) are not linked into the game, so their licenses do not
affect the game's licenses.

## Inspirations

Each level is a homage to a classic game. The inspiring games, their names and
their trademarks belong to their owners. This project uses none of their
assets, code, characters or level layouts. The list is in
`docs/concept/levels/README.md`.
