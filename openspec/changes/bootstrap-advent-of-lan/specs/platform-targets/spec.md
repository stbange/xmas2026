# Spec Delta

## Purpose
Where the game runs and how it is delivered: native Windows and Linux builds, a browser build for desktop and mobile, WebXR, and the gh-pages playtest site.

## ADDED Requirements

### Requirement: Native builds
The game SHALL build and run on Windows x64 and Linux x64 with OpenGL 3.3 from the same source code.

#### Scenario: Release artefacts
- **WHEN** a release is tagged
- **THEN** CI produces a Windows zip and a Linux tarball that start the game

### Requirement: Web build
The game SHALL run in current desktop and mobile versions of Chrome, Firefox and Safari with WebGL 2, without plug-ins or installation.

#### Scenario: First load
- **WHEN** the web build is opened for the first time
- **THEN** the engine and hall (at most 10 MB compressed) load, and the start screen asks for a tap or key press to enable audio

### Requirement: Web saves persist
The web build SHALL store saves and settings in browser storage so they survive reloads and browser restarts.

#### Scenario: Reload keeps settings
- **WHEN** the player changes the FOV and reloads the page
- **THEN** the FOV setting is preserved

### Requirement: WebXR
When the browser supports immersive VR, the web build SHALL show an "Enter VR" button and SHALL run the hall and all levels in VR with stereo rendering and controller input; when unsupported, the button SHALL be hidden.

#### Scenario: Unsupported browser
- **WHEN** the browser does not support immersive VR sessions
- **THEN** no VR button is shown

#### Scenario: Comfort warning
- **WHEN** an XR player enters a level rated Intense
- **THEN** a comfort warning is shown with the option to enable the comfort settings

### Requirement: gh-pages site
Every successful build of the main branch SHALL be published to the gh-pages branch: the release web build at the site root (for players) and the playtest web build under `/playtest/` (for testing). Both SHALL display their build identifier on the start screen.

#### Scenario: Publish after green CI
- **WHEN** a commit on main passes all CI gates
- **THEN** the gh-pages branch contains the release web build of that commit at its root and the playtest build under `playtest/`, and both start screens show its short hash

#### Scenario: Red CI does not publish
- **WHEN** a commit on main fails any CI gate
- **THEN** the gh-pages branch is not updated
