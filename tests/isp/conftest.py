# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import subprocess

from pathlib import Path

cwd = Path(__file__).parent.absolute()

def pytest_sessionstart(session):
    print("\n===================================")
    print("Running cmake and xmake")
    cmake_cmd = 'cmake -G "Unix Makefiles" -B build --fresh'
    build_cmd = 'xmake -C build'
    subprocess.run(cmake_cmd, shell=True, cwd=cwd, check=True)
    subprocess.run(build_cmd, shell=True, cwd=cwd, check=True)
    print("===================================")
