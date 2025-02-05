# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
from pathlib import Path


def postprocess_results(session):
    """Collect and display all test results at the end."""
    print("\n" + "=" * 80 + "\n")
    print("Collected Test Results:")
    for result in session.results:
        print(result)


def remove_tmp_bin_files():
    """Remove all temporary binary files in the current working directory."""
    cwd = Path(__file__).parent.absolute()
    for file in cwd.rglob("*.tmp.bin"):
        file.unlink()


def pytest_sessionstart(session):
    """Initialize an empty list for storing test results at session start."""
    session.results = []


@pytest.hookimpl(tryfirst=True)
def pytest_sessionfinish(session, exitstatus):
    remove_tmp_bin_files()
    postprocess_results(session)
