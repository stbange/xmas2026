# 04 — Art & Audio Direction

## The look in one sentence
**"How we remember 1996 looked"**: chunky pixels, limited palettes, sprites
and voxels, but lit by modern dynamic lights and soft-edged, low-resolution
shadows, at a smooth 60 fps.

## Resolution & pixels
- The 3D scene renders into a **low-resolution internal framebuffer** and is
  upscaled with nearest-neighbour to the screen.
  | Preset | Internal (16:9) | Scales to |
  |---|---|---|
  | *Crunchy* | 320×180 | ×6 1080p, ×8 1440p, ×12 4K |
  | *Classic* (default) | 480×270 | ×4 1080p, ×8 4K |
  | *Crisp* | 640×360 | ×3 1080p, ×4 1440p, ×6 4K |
  Other aspect ratios keep the internal height and adapt the width.
- The **HUD and text** render at a higher "UI resolution" (2× internal) with
  pixel fonts, so text stays readable.
- **XR** uses a higher internal resolution per eye (low resolution in a headset
  feels blurry and causes discomfort); the retro look comes from textures,
  palettes and nearest filtering instead.

## Palettes
- Every level has its **own 64–256-colour palette** that evokes its original
  (Quake brown, Doom red-brown, C&C sand and green, Syndicate neon …).
- The final image is **quantised to the level's palette** through a 3D colour
  lookup table, with optional **ordered (Bayer) dithering**. This makes
  dynamic lighting "feel" 8-bit: lights produce colour bands, not smooth gradients.
- **Light ramps:** diminishing light (Doom-style banding into darkness) is
  done through the palette LUT, not through fog.
- Players can turn palette quantisation down (accessibility).

## Building blocks
| Building block | Used for | Notes |
|---|---|---|
| **Brush/sector geometry** | architecture | Built in TrenchBroom (Quake `.map` format), compiled by our tool. Low-poly, 64–128 px textures, nearest filtering. |
| **Sprites** | enemies (Doom/Duke/Blood/Diablo style), pickups, particles, NPCs | 8-direction billboards; lit by dynamic lights using a per-sprite normal map (optional); palette swaps for team colours. |
| **Voxels** | vehicles, buildings, destructible objects, the tree, KRAMPUS.EXE | MagicaVoxel `.vox` (or Goxel) → greedy-meshed with baked AO; destructible voxels re-meshed per 16³ chunk. |
| **Low-poly 3D models** | mechs, player hands/weapons, big monsters | glTF 2.0; skeletal animation, plus **vertex-morph** animation for the Quake homage. |
| **Heightfield terrain** | days 2, 6, 9, 21, 23 | Deformable; vertex colour + splat textures. |

## Lighting & shadows
- **Dynamic lights everywhere:** muzzle flashes, fireballs, flares, neon, tree
  lights. The renderer handles dozens of lights per view (see
  [technology](05-technology.md)).
- **Shadows:** one directional light (sun/moon) with a shadow map, plus up to
  4 point/spot lights with shadows per frame (cube maps at 128² per face). The
  low resolution of shadow maps is part of the look: soft, chunky edges.
- **Lightmaps** (baked) for static lighting in levels that imitate
  lightmapped games (Quake, Half-Life, Counter-Strike, Unreal), combined
  with dynamic lights.
- **Emissive / "fullbright"** pixels that ignore lighting (glowing eyes,
  screens, lava) — a Quake-era signature.
- **Fog:** distance fog and coloured volumetric fog volumes (Unreal homage).

## Optional retro filters (player options)
- *PS1 wobble*: affine texture mapping and vertex snapping (off by default).
- *CRT*: scanlines, curvature, slight bloom (off by default).
- *Palette*: on/off, dithering on/off.

## Art production rules
- **No assets from the original games.** Not even as placeholders. Every texture,
  sprite, model and sound is made for this project (or is CC0/CC BY from
  attributed sources).
- Each level has an **art bible page** (palette, 5–10 reference images of the
  *genre* — not ripped screenshots — and a list of homage elements).
- Texture size: 64×64 or 128×128 (walls), 32×32 details; sprites 64–128 px
  tall for humans.
- All source files (`.aseprite`/`.png`, `.vox`, `.blend`, `.map`) live in
  `assets/` with their exports; licence CC BY 4.0; attribution in
  `assets/ATTRIBUTION.md`.

## Audio
- **Music:** each level has a track in the *style* of its homage (MIDI-ish rock
  for Duke, ambient-industrial for Quake, tracker module for Unreal, orchestra
  for TA, march for Red Alert), all original and mixed with a Christmas motif.
  The hall has a main theme, "The Great Hall", which gets more instruments as
  more baubles are found.
- **Formats:** OGG Vorbis (stb_vorbis) for streams; optionally **tracker
  modules** (`.it`/`.xm`) for the Unreal and hall themes — authentic and tiny.
  (Before using a module player library, check its licence for MIT compatibility.)
- **SFX:** chunky, 22 kHz-feel sound effects; materials for footsteps; 3D
  positional audio with occlusion (low-pass behind walls).
- **Voices:** NOEL: short blips + text. KRAMPUS.EXE: text + a few heavily
  processed, generated lines (day 20, 24). Avatar one-liners (day 1): text
  bubbles with a short "voice" sound. Everything subtitled.
- **Music licence:** CC BY 4.0 like all art; composers credited in `CREDITS.md`.
