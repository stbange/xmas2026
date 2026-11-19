# ADVENT OF LAN — 24 Portals to 2001

A first-person, retro-3D advent calendar for gamers who were there.

You walk through a great, snow-dusted festive hall with **24 portals**. From
**1 December to 24 December 2026** one more portal opens every day. Behind each
one is a ~20-minute level that pays homage to a game we played at LAN parties
25 years ago: from a neon shopping mall full of alien boars on day 1 to the
final war on day 24.

| | |
|---|---|
| **Genre** | First-person action / puzzle, co-op and versus |
| **Look** | Low-res, palette-quantised retro 3D: polygons, sprites, voxels, with dynamic lights and shadows |
| **Tech** | C17 · SDL3 · OpenGL 3.3 / GLES 3 / WebGL 2 |
| **Platforms** | Windows · Linux · Web (Emscripten, touch on mobile) · WebXR |
| **Multiplayer** | Split screen (up to 4) and online (up to 4), including cross-play with the web version |
| **Process** | OpenSpec spec-driven development, ≥95 % unit-test coverage, automated game tests |
| **Playtest** | Web build on the `gh-pages` branch |

## Where to start reading

- **Concept / game design:** [`docs/concept/`](docs/concept/README.md)
  - [Vision & pillars](docs/concept/00-vision.md)
  - [Meta story](docs/concept/01-meta-story.md)
  - [The Hall & the calendar](docs/concept/02-the-hall.md)
  - [Core gameplay](docs/concept/03-core-gameplay.md)
  - [The 24 levels](docs/concept/levels/README.md)
  - [Art & audio direction](docs/concept/04-art-and-audio.md)
  - [Technology](docs/concept/05-technology.md)
  - [Multiplayer](docs/concept/06-multiplayer.md)
  - [Platforms, touch & WebXR](docs/concept/07-platforms-input-xr.md)
  - [Quality & testing](docs/concept/08-quality-and-testing.md)
  - [Production plan](docs/concept/09-production-plan.md)
  - [Legal & homage guidelines](docs/concept/10-legal-and-homage.md)
- **Specs (OpenSpec):** [`openspec/`](openspec/) — the change
  [`bootstrap-advent-of-lan`](openspec/changes/bootstrap-advent-of-lan/proposal.md)
  holds the requirements and the first implementation tasks.

## Licenses

- **Code:** [MIT](LICENSE)
- **Art and content** (textures, sprites, voxels, models, maps, audio, music, story text):
  [CC BY 4.0](LICENSE-ART). See [CREDITS.md](CREDITS.md).

This is a non-commercial fan homage. All names, characters, art, audio and
levels are original. Mechanics and genres are the homage, not anyone's assets;
see [legal & homage guidelines](docs/concept/10-legal-and-homage.md). The games
that inspired each level are trademarks of their respective owners.
