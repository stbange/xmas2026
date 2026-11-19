# 07 — Platforms, Input & WebXR

## Platforms
| Platform | Toolchain | Package | Notes |
|---|---|---|---|
| **Windows** x64 | MSVC 2022 or MinGW-w64 (clang) | zip with `.exe`, content `.pak`s | GL 3.3 |
| **Linux** x64 | GCC 13+/Clang 17+ | tar.gz (AppImage later) | GL 3.3; Wayland and X11 via SDL3 |
| **Web** | Emscripten (pinned version) | static files on `gh-pages` | WebGL 2, WebAudio, IndexedDB, Gamepad API, touch |
| **WebXR** | same build as Web | — | `immersive-vr` session when available |

All platforms build from the same source; platform differences are in
`src/platform/` and `web/`.

### Web build specifics
- SDL3 is built from source with `emcmake` (SDL3 supports Emscripten).
- The main loop uses `emscripten_set_main_loop` / `requestAnimationFrame`.
- Files: the hall content is preloaded; level packs are fetched on demand
  (and cached in IndexedDB).
- Audio starts after the first user gesture (browser policy) — the BIOS gate
  screen has a "Press any key / Tap to start" for this.
- Target size: < 10 MB for the first download (engine + hall), each level
  pack 2–10 MB.
- The page is a small, responsive HTML shell (`web/shell.html`) with a
  fullscreen button, an "Enter VR" button (only shown when WebXR VR is
  supported), and a touch-control overlay.

## Input
- SDL3 handles keyboard, mouse (relative mode/pointer lock), gamepads and
  touch on all platforms.
- An **action map** turns device input into game actions (move, look,
  fire, …); **levels declare their extra actions** in an *input schema*
  (e.g. `jetpack`, `ski`, `heat_vent`, `tactical`), so every device gets a
  binding and touch/XR show the right buttons.
- Rebinding for keyboard/mouse and gamepad; presets for left-handed players.
- **Aim assist** (slow-down + slight magnetism) for gamepad and touch,
  adjustable, never in versus unless everyone has it.

## Touch controls (mobile web)
**Switching:** touch mode switches on automatically when the device has a
coarse pointer (`(pointer: coarse)`) **and** the first touch arrives; it
switches off again as soon as keyboard, mouse or gamepad input arrives. The
player can also force it in the options.

**Layout (landscape):**
```
 ┌──────────────────────────────────────────────────────────────┐
 │ ☰  map                                  [ammo] [weapon ◄ ►]  │
 │                                                               │
 │                                                  (ability 2)  │
 │       (floating                                  (ability 1)  │
 │        move stick)            look: drag anywhere             │
 │                               on the right half   (USE)       │
 │                                                   (JUMP)      │
 │                                          (CROUCH)  ( FIRE )   │
 └──────────────────────────────────────────────────────────────┘
```
- **Left half:** a floating virtual stick that appears where the thumb lands;
  double-tap and push to sprint.
- **Right half:** drag to look (with acceleration curve and sensitivity
  settings); optional **gyro aiming** (Device Orientation API, asks for
  permission on iOS).
- **Fire** button; option "auto-fire when the crosshair is on an enemy".
- **Use** only appears when something is usable; **ability** buttons come
  from the level's input schema (jetpack, ski, spell…).
- Weapon switch: swipe on the weapon icon or tap for a radial menu.
- UI scale, button opacity, left-handed mirroring; safe-area insets.
- Portrait orientation shows "please rotate" (and pauses).
- Mobile performance: *Crunchy* preset, fewer shadow-casting lights, fewer
  particles, dynamic resolution scaling.

## WebXR
- **Session:** `immersive-vr` with `local-floor` reference space; the
  "Enter VR" button appears only when `navigator.xr.isSessionSupported`
  says yes.
- **Integration:** a small JavaScript library (in `web/`) wraps the WebXR API
  and calls into the C side each XR frame with the view/projection matrices,
  viewports and controller poses. The renderer draws both eyes into the
  `XRWebGLLayer` framebuffer (using the same pipeline, rendering the
  low-res passes per eye).
- **Look:** a higher internal resolution per eye (e.g. 50 % of the native eye
  resolution); keep palette quantisation and nearest-neighbour texture
  filtering for the retro feel; no CRT or wobble filters.
- **Locomotion & comfort:** smooth locomotion (thumbstick) with optional
  **teleport** in the hall; **snap turn** (default 30°) or smooth turn;
  **vignette** during movement; seated mode; height calibration.
- **Controls:** trigger = fire, grip = use/grab, A/X = jump, B/Y = ability,
  thumbstick click = sprint; weapons are aimed with the hand (6DOF
  controllers); the **NOEL-Pad** is on the left wrist; the day-14 ring menu
  becomes a ring around the hand.
- **Comfort rating per level:** *Comfortable* (most), *Moderate* (vehicles:
  2, 6, 9), *Intense* (6DOF: 11, 20's cyberspace). Intense levels show a
  warning and enable a cockpit frame and auto-level.
- Day 4's third-person mode and the overhead "drone-cam" (days 3, 10, 15) are
  shown as a **diorama** in front of the player in VR.
- Hand tracking: not in scope (controllers required).
- Desktop VR via OpenXR: out of scope for 2026.
