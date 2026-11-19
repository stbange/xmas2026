#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Attribution check: every file under assets/ needs an entry in
assets/ATTRIBUTION.md (a table row whose first cell is the path in
backticks, relative to assets/), and every entry must name an existing file
and an allowed licence.

Usage: check_attribution.py
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ASSETS = os.path.join(ROOT, "assets")
INDEX = os.path.join(ASSETS, "ATTRIBUTION.md")
IGNORED = {"ATTRIBUTION.md", "README.md", ".gitkeep"}
ROW_RE = re.compile(r"^\|\s*`([^`]+)`\s*\|([^|]*)\|([^|]*)\|")
ALLOWED_LICENCES = {"CC BY 4.0", "CC0 1.0", "CC0"}


def asset_files(base):
    out = set()
    for dirpath, _, files in os.walk(base):
        for name in files:
            if name in IGNORED or name.startswith("."):
                continue
            out.add(os.path.relpath(os.path.join(dirpath, name), base).replace(os.sep, "/"))
    return out


def entries(index):
    rows = {}
    if not os.path.exists(index):
        return rows
    with open(index, encoding="utf-8") as f:
        for line in f:
            m = ROW_RE.match(line.strip())
            if m:
                rows[m.group(1).strip()] = (m.group(2).strip(), m.group(3).strip())
    return rows


def check(base=ASSETS, index=INDEX):
    files = asset_files(base)
    rows = entries(index)
    errors = []
    for f in sorted(files - set(rows)):
        errors.append(f"missing attribution entry for assets/{f}")
    for f in sorted(set(rows) - files):
        errors.append(f"attribution entry for missing file assets/{f}")
    for f, (_, licence) in sorted(rows.items()):
        if licence not in ALLOWED_LICENCES:
            errors.append(f"assets/{f}: licence '{licence}' not allowed (use one of {sorted(ALLOWED_LICENCES)})")
    return files, errors


def main():
    files, errors = check()
    print(f"attribution: {len(files)} asset files checked")
    for e in errors:
        print("ERROR:", e)
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
