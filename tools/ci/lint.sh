#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
# Lint: clang-format check, clang-tidy and cppcheck over game C code.
# Needs a configured build with compile_commands.json (default build/linux-debug).
set -euo pipefail
cd "$(dirname "$0")/../.."
BUILD="${1:-build/linux-debug}"
mapfile -t FILES < <(git ls-files 'src/*.c' 'src/*.h' 'tests/*.c' 'tests/*.h' 'tools/*.c' 'tools/*.h')
echo "clang-format: ${#FILES[@]} files"
clang-format --dry-run --Werror "${FILES[@]}"
mapfile -t CFILES < <(git ls-files 'src/*.c' 'tools/*.c')
echo "clang-tidy: ${#CFILES[@]} files"
clang-tidy -p "$BUILD" --quiet "${CFILES[@]}"
echo "cppcheck"
cppcheck --error-exitcode=1 --enable=warning,performance,portability --inline-suppr \
  --suppress=missingIncludeSystem --std=c11 -I src --quiet src
echo "lint OK"
