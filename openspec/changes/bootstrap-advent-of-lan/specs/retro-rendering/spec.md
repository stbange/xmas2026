# Spec Delta

## Purpose
The game's visual identity: a low-resolution, palette-limited retro 3D look built from polygons, sprites and voxels, lit by dynamic lights with shadows, at modern frame rates.

## ADDED Requirements

### Requirement: Low internal resolution
The 3D scene SHALL be rendered at a low internal resolution (presets 320×180, 480×270 default, 640×360 for 16:9, height kept for other aspect ratios) and upscaled to the window with nearest-neighbour filtering; HUD text SHALL render at a higher UI resolution.

#### Scenario: Default preset at 1080p
- **WHEN** the window is 1920×1080 with the default preset
- **THEN** the scene renders at 480×270 and each scene pixel covers exactly 4×4 screen pixels

### Requirement: Level palettes
Each level SHALL define its own palette of up to 256 colours, and the final image SHALL be quantised to it (with optional ordered dithering) unless the player turns quantisation off.

#### Scenario: Quantised output
- **WHEN** palette quantisation is on
- **THEN** every pixel of the scene image is a colour from the level's palette

### Requirement: Mixed primitives
The renderer SHALL draw brush/mesh geometry, 8-direction sprites, voxel models and skeletal or vertex-morph animated low-poly models in the same scene, all lit by the same lights.

#### Scenario: Sprite lit by a muzzle flash
- **WHEN** a weapon fires next to a sprite enemy in a dark room
- **THEN** the sprite is visibly lit by the muzzle flash

### Requirement: Dynamic lights and shadows
The renderer SHALL support at least 32 dynamic point/spot lights in view, one shadowed directional light, and at least 4 shadow-casting point/spot lights per frame on desktop (fewer allowed on mobile automatic quality).

#### Scenario: Moving shadow
- **WHEN** a shadow-casting light moves past a voxel object
- **THEN** the object's shadow moves accordingly in the same frame

### Requirement: Destructible voxels
Voxel objects marked destructible SHALL lose voxels where they are hit, update their visuals and collision, and break into separate pieces when disconnected.

#### Scenario: Wall destroyed
- **WHEN** an explosion removes the voxels connecting the top of a voxel building to its base
- **THEN** the top part falls as a separate piece

### Requirement: Frame-rate targets
The game SHALL reach 60 fps at the default preset on the desktop reference GPU (2018 integrated class) in native and web builds, at least 30 fps on the mobile reference phone with automatic quality, and the headset's native rate (≥72 fps) on the XR reference headset with XR quality.

#### Scenario: Desktop web performance
- **WHEN** level 1's busiest scene runs in Chrome on the desktop reference machine
- **THEN** the 95th-percentile frame time is at most 16.7 ms

### Requirement: Optional retro filters
The player SHALL be able to toggle CRT effect, affine texture wobble, palette quantisation and dithering; XR mode SHALL disable CRT and wobble.

#### Scenario: CRT in XR
- **WHEN** the CRT filter is on and the player enters XR
- **THEN** the CRT filter is not applied in the headset
