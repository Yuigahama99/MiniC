#!/usr/bin/env python3
"""
run_tests.py - Master test runner
Discovers all *.mc files under tests/<suite>/ and runs them against the binary.
Files prefixed with "error_" are expected to produce an "Error" token.
"""

import os
import sys
import subprocess

ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEST_DIR = os.path.join(ROOT_DIR, "tests")


def resolve_binary_path(root_dir):
    """Resolve project binary path for the CMake/macOS workflow."""
    candidate = os.path.join(root_dir, "build", "minic")
    if os.path.isfile(candidate):
        return candidate
    return candidate


BINARY = resolve_binary_path(ROOT_DIR)

# ANSI colors
GREEN  = "\033[92m"
RED    = "\033[91m"
CYAN   = "\033[96m"
DARK_RED = "\033[31m"
RESET  = "\033[0m"

def run_test(binary, filepath):
    """Run binary on filepath, return (exit_code, output_lines)."""
    import tempfile
    with tempfile.NamedTemporaryFile(mode='r', suffix='.txt', delete=False) as tmp:
        tmpname = tmp.name

    try:
        with open(tmpname, 'w') as out:
            proc = subprocess.run(
                [binary, filepath],
                stdout=subprocess.DEVNULL,
                stderr=out,
                timeout=10
            )
        with open(tmpname, 'r') as f:
            lines = [l.rstrip() for l in f if l.strip()]
        return proc.returncode, lines
    finally:
        try:
            os.remove(tmpname)
        except OSError:
            pass


def run_suite(suite_dir):
    test_files = sorted(
        f for f in os.listdir(suite_dir) if f.endswith(".mc")
    )
    if not test_files:
        return 0, 0

    # create output directory alongside the test files
    output_dir = os.path.join(suite_dir, "output")
    os.makedirs(output_dir, exist_ok=True)

    passed = 0
    failed = 0

    for filename in test_files:
        filepath = os.path.join(suite_dir, filename)
        expect_error = filename.startswith("error_")

        try:
            exit_code, output = run_test(BINARY, filepath)
        except subprocess.TimeoutExpired:
            print(f"  {RED}[TIMEOUT]{RESET} {filename}")
            failed += 1
            continue
        except Exception as e:
            print(f"  {RED}[CRASH]  {RESET}{filename} ({e})")
            failed += 1
            continue

        # write output to file
        out_filename = os.path.splitext(filename)[0] + ".out"
        out_filepath = os.path.join(output_dir, out_filename)
        with open(out_filepath, 'w') as f:
            f.write("\n".join(output) + "\n")

        has_error = any("[error]" in line for line in output)

        if exit_code != 0:
            print(f"  {RED}[CRASH]  {RESET}{filename} (exit {exit_code})")
            failed += 1
        elif expect_error and has_error:
            print(f"  {GREEN}[PASS]   {RESET}{filename} (expected Error token)")
            passed += 1
        elif expect_error and not has_error:
            print(f"  {RED}[FAIL]   {RESET}{filename} (expected Error token, got none)")
            failed += 1
        elif not expect_error and has_error:
            print(f"  {RED}[FAIL]   {RESET}{filename} (unexpected Error token)")
            for line in output:
                if "Type: Error" in line:
                    print(f"  {DARK_RED}         {line}{RESET}")
            failed += 1
        else:
            print(f"  {GREEN}[PASS]   {RESET}{filename}")
            passed += 1

    return passed, failed


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--suite", default=None, help="Run only this suite (e.g. lexer)")
    args = parser.parse_args()

    if not os.path.isfile(BINARY):
        print(f"{RED}Binary not found: {BINARY}{RESET}")
        sys.exit(1)

    total_passed = 0
    total_failed = 0

    suites = sorted(
        d for d in os.listdir(TEST_DIR)
        if os.path.isdir(os.path.join(TEST_DIR, d))
        and (args.suite is None or d == args.suite)
    )

    for suite in suites:
        suite_dir = os.path.join(TEST_DIR, suite)
        test_files = [f for f in os.listdir(suite_dir) if f.endswith(".mc")]
        if not test_files:
            continue

        print(f"{CYAN}======================================{RESET}")
        print(f"{CYAN} Suite: {suite}{RESET}")
        print(f"{CYAN}======================================{RESET}")

        passed, failed = run_suite(suite_dir)
        color = GREEN if failed == 0 else RED
        print(f"  {color}Results: {passed} passed, {failed} failed{RESET}\n")

        total_passed += passed
        total_failed += failed

    print(f"{CYAN}======================================{RESET}")
    color = GREEN if total_failed == 0 else RED
    print(f"{color} TOTAL: {total_passed} passed, {total_failed} failed{RESET}")
    print(f"{CYAN}======================================{RESET}")

    sys.exit(0 if total_failed == 0 else 1)


if __name__ == "__main__":
    main()
