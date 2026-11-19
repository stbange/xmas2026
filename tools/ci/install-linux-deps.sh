#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
# Packages needed to build SDL3 + the game and run the tests on Ubuntu 24.04.
set -euo pipefail
sudo apt-get update -q
sudo apt-get install -y -q --no-install-recommends \
  ninja-build clang libclang-rt-18-dev clang-tidy clang-format cppcheck xvfb \
  libgl-dev libegl-dev libgles-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev \
  libxi-dev libxss-dev libxtst-dev libwayland-dev libxkbcommon-dev libdecor-0-dev \
  libgl1-mesa-dri mesa-utils
pip3 install --user --break-system-packages gcovr==8.8 >/dev/null 2>&1 || pip3 install --user gcovr==8.8
# pip --user installs to ~/.local/bin; make sure later CI steps find gcovr.
if [ -n "${GITHUB_PATH:-}" ]; then echo "$HOME/.local/bin" >> "$GITHUB_PATH"; fi
