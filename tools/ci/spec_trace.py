#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Spec-trace check: every OpenSpec scenario must be referenced by a test.

A test references a scenario with a tag anywhere in a file under tests/:

    /* @scenario advent-calendar: Portal opens at local midnight */

Scenarios that are specified but not implemented yet are listed in
openspec/trace-pending.txt ("<capability>: <Scenario name>" per line). The
check fails when:
  * a scenario is neither tested nor pending,
  * a pending entry is already tested (remove it from the list),
  * a pending entry or a test tag names a scenario that does not exist.

Usage: spec_trace.py [--report]
"""
import argparse
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SCENARIO_RE = re.compile(r"^####\s+Scenario:\s*(.+?)\s*$")
TAG_RE = re.compile(r"@scenario\s+([a-z0-9][a-z0-9/-]*)\s*:\s*(.+?)\s*(?:\*/|$)", re.MULTILINE)
TEST_EXT = (".c", ".h", ".py", ".js", ".mjs", ".ts", ".lua")


def norm(capability, name):
    return capability.strip().lower() + ": " + " ".join(name.strip().lower().split())


def spec_files(root):
    """Yields (capability, path) for main specs and active (non-archived) change deltas."""
    main = os.path.join(root, "openspec", "specs")
    changes = os.path.join(root, "openspec", "changes")
    for base in (main,):
        for dirpath, _, files in os.walk(base):
            if "spec.md" in files:
                yield os.path.relpath(dirpath, base).replace(os.sep, "/"), os.path.join(dirpath, "spec.md")
    if os.path.isdir(changes):
        for change in sorted(os.listdir(changes)):
            if change == "archive":
                continue
            sbase = os.path.join(changes, change, "specs")
            for dirpath, _, files in os.walk(sbase):
                if "spec.md" in files:
                    yield os.path.relpath(dirpath, sbase).replace(os.sep, "/"), os.path.join(dirpath, "spec.md")


def collect_scenarios(root):
    scenarios = {}
    for cap, path in spec_files(root):
        with open(path, encoding="utf-8") as f:
            for line in f:
                m = SCENARIO_RE.match(line)
                if m:
                    scenarios[norm(cap, m.group(1))] = f"{cap}: {m.group(1)}"
    return scenarios


def collect_tags(root):
    tags = {}
    tests = os.path.join(root, "tests")
    for dirpath, dirs, files in os.walk(tests):
        dirs[:] = [d for d in dirs if d not in ("node_modules", "test-results")]
        for name in files:
            if not name.endswith(TEST_EXT):
                continue
            path = os.path.join(dirpath, name)
            with open(path, encoding="utf-8", errors="replace") as f:
                for m in TAG_RE.finditer(f.read()):
                    tags.setdefault(norm(m.group(1), m.group(2)), []).append(os.path.relpath(path, root))
    return tags


def collect_pending(root):
    path = os.path.join(root, "openspec", "trace-pending.txt")
    pending = {}
    if os.path.exists(path):
        with open(path, encoding="utf-8") as f:
            for n, line in enumerate(f, 1):
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                cap, _, name = line.partition(":")
                pending[norm(cap, name)] = n
    return pending


def check(root):
    scenarios = collect_scenarios(root)
    tags = collect_tags(root)
    pending = collect_pending(root)
    errors = []
    for key, label in sorted(scenarios.items()):
        if key not in tags and key not in pending:
            errors.append(f"untested scenario (add a test tag or list it as pending): {label}")
        if key in tags and key in pending:
            errors.append(f"stale pending entry, scenario is tested now: {label}")
    for key, files in sorted(tags.items()):
        if key not in scenarios:
            errors.append(f"test tag names unknown scenario '{key}' in {', '.join(sorted(set(files)))}")
    for key, line in sorted(pending.items()):
        if key not in scenarios:
            errors.append(f"trace-pending.txt:{line} names unknown scenario '{key}'")
    tested = sum(1 for k in scenarios if k in tags)
    return scenarios, tested, errors


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--report", action="store_true", help="list tested scenarios")
    ap.add_argument("--root", default=ROOT)
    args = ap.parse_args(argv)
    scenarios, tested, errors = check(args.root)
    if args.report:
        tags = collect_tags(args.root)
        for key, label in sorted(scenarios.items()):
            print(("[x] " if key in tags else "[ ] ") + label)
    print(f"spec trace: {tested}/{len(scenarios)} scenarios tested")
    for e in errors:
        print("ERROR:", e)
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
