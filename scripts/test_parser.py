#!/usr/bin/env python3
"""test_parser.py - Run only the parser test suite."""
import os, sys
sys.argv += ["--suite", "parser"]
exec(open(os.path.join(os.path.dirname(__file__), "run_tests.py")).read())
