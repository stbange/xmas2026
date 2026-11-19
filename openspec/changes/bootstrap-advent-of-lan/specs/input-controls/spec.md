# Spec Delta

## Purpose
How players control the game with keyboard and mouse, gamepads, touch screens and XR controllers, including automatic switching to touch controls on mobile web.

## ADDED Requirements

### Requirement: Supported devices
The game SHALL be fully playable with keyboard and mouse, with a gamepad, with touch (web on mobile devices), and with XR controllers (WebXR).

#### Scenario: Gamepad only
- **WHEN** a player uses only a gamepad from the start screen
- **THEN** they can navigate menus, the hall and finish level 1

### Requirement: Automatic touch mode
On the web, touch controls SHALL appear automatically when the device has a coarse pointer and a touch occurs, and SHALL hide when keyboard, mouse or gamepad input occurs; the player SHALL be able to force touch mode on or off.

#### Scenario: Phone
- **WHEN** the game is opened on a phone and the player touches the screen
- **THEN** the virtual move stick, look area and action buttons are shown

#### Scenario: Tablet with keyboard
- **WHEN** touch mode is active and a hardware key is pressed
- **THEN** the touch overlay hides

### Requirement: Touch layout
Touch mode SHALL provide a floating move stick on the left half, drag-to-look on the right half, fire, jump, crouch and a contextual use button, a weapon switcher, and buttons for the current level's actions; it SHALL respect safe-area insets and offer optional gyro aiming, button scale/opacity and a left-handed layout.

#### Scenario: Contextual use
- **WHEN** the player looks at a usable switch in touch mode
- **THEN** the Use button appears, and it disappears when nothing is usable

### Requirement: Rebinding
All actions, including level actions, SHALL be rebindable for keyboard/mouse and gamepad, and bindings SHALL be saved.

#### Scenario: Rebinding jump
- **WHEN** the player binds Jump to the right mouse button and restarts the game
- **THEN** Jump is still on the right mouse button

### Requirement: Aim assist
Gamepad and touch players SHALL get adjustable aim assist; in versus modes aim assist SHALL be off unless the host enables it for everyone.

#### Scenario: Versus default
- **WHEN** a versus match starts with default host settings
- **THEN** aim assist is disabled for all players

### Requirement: XR controls
In XR the player SHALL aim with the controller, fire with the trigger, use/grab with the grip, move with the thumbstick, turn with snap turn (default) or smooth turn, and open the NOEL-Pad on the wrist.

#### Scenario: Snap turn
- **WHEN** an XR player pushes the right thumbstick to the right with default settings
- **THEN** the view turns 30° to the right in one step
