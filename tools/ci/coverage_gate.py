#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Coverage gate for Advent of LAN.

Runs gcovr over a coverage build and enforces:
  * line coverage   >= 95 % of game C code
  * branch coverage >= 90 % of game C code
  * excluded code (platform entry point, raw GL backend) < 5 % of game C lines

Usage: coverage_gate.py --build build/linux-coverage [--html out/coverage.html]
"""
import argparse
import json
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# The ONLY directories allowed to be excluded from the metric (see
# openspec quality-gates: "Unit-test coverage gate").
EXCLUDED_DIRS = ("src/platform/entry/", "src/render/gl/")

LINE_MIN = 95.0
BRANCH_MIN = 90.0
EXCLUDED_MAX_RATIO = 0.05


def count_code_lines(path):
    """Counts non-blank lines that are not pure comment lines."""
    n = 0
    in_block = False
    with open(path, encoding="utf-8", errors="replace") as f:
        for raw in f:
            line = raw.strip()
            if in_block:
                if "*/" in line:
                    in_block = False
                    line = line.split("*/", 1)[1].strip()
                else:
                    continue
            if not line or line.startswith("//"):
                continue
            if line.startswith("/*"):
                if "*/" not in line:
                    in_block = True
                    continue
                line = line.split("*/", 1)[1].strip()
                if not line:
                    continue
            n += 1
    return n


def game_c_files(root):
    out = []
    for dirpath, _, files in os.walk(os.path.join(root, "src")):
        for name in files:
            if name.endswith((".c", ".h")):
                out.append(os.path.relpath(os.path.join(dirpath, name), root).replace(os.sep, "/"))
    return sorted(out)


def excluded_ratio(root, files=None):
    files = game_c_files(root) if files is None else files
    total = excluded = 0
    for rel in files:
        n = count_code_lines(os.path.join(root, rel))
        total += n
        if rel.startswith(EXCLUDED_DIRS):
            excluded += n
    return (excluded / total) if total else 0.0, excluded, total


def run_gcovr(build, html):
    cmd = [
        "gcovr", "--root", ROOT, "--object-directory", build,
        "--filter", os.path.join(ROOT, "src") + "/",
        "--exclude-unreachable-branches", "--exclude-throw-branches",
        "--json-summary-pretty", "--json-summary", os.path.join(build, "coverage-summary.json"),
        "--txt", os.path.join(build, "coverage.txt"),
    ]
    for d in EXCLUDED_DIRS:
        cmd += ["--exclude", os.path.join(ROOT, d)]
    if html:
        os.makedirs(os.path.dirname(os.path.abspath(html)), exist_ok=True)
        cmd += ["--html-details", html]
    subprocess.run(cmd, check=True)
    with open(os.path.join(build, "coverage-summary.json"), encoding="utf-8") as f:
        return json.load(f)


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--build", required=True)
    ap.add_argument("--html")
    args = ap.parse_args(argv)

    summary = run_gcovr(os.path.abspath(args.build), args.html)
    with open(os.path.join(args.build, "coverage.txt"), encoding="utf-8") as f:
        print(f.read())
    line = float(summary.get("line_percent") or 0.0)
    branch_total = int(summary.get("branch_total") or 0)
    branch = float(summary.get("branch_percent") or 0.0) if branch_total else 100.0
    ratio, excl, total = excluded_ratio(ROOT)

    ok = True
    print(f"line coverage   : {line:6.2f} %  (min {LINE_MIN} %)")
    print(f"branch coverage : {branch:6.2f} %  (min {BRANCH_MIN} %)")
    print(f"excluded code   : {100 * ratio:6.2f} %  ({excl}/{total} lines, max {100 * EXCLUDED_MAX_RATIO:.0f} %)")
    if line < LINE_MIN:
        print("FAIL: line coverage below threshold")
        ok = False
    if branch < BRANCH_MIN:
        print("FAIL: branch coverage below threshold")
        ok = False
    if ratio >= EXCLUDED_MAX_RATIO:
        print("FAIL: excluded platform/GL code is too large; move logic into tested modules")
        ok = False
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
