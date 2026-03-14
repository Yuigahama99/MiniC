#!/usr/bin/env python3
"""test_lexer.py - Run only the lexer test suite."""
import os, sys
sys.argv += ["--suite", "lexer"]
exec(open(os.path.join(os.path.dirname(__file__), "run_tests.py")).read())
