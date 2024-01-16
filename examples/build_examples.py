# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

# This file builds all examples in the examples folder within a single script.

import shutil
import subprocess
from string import Template
from pathlib import Path

generator = "Ninja" # or "Unix Makefiles"
make = "ninja" # or "xmake"

cwd = Path(__file__).parent.absolute()
cmake_template = Template(f'cmake -G {generator} -S $src_dir -B $src_dir/build')
xmake_template = Template(f'{make} -C $src_dir/build')

# List of folders
examples = [
    "take_picture_downsample",
    "take_picture_local",
    "take_picture_raw"
]

# Run each command
for example in examples:
    src_dir = cwd / example
    # remove build and dir folders
    shutil.rmtree(src_dir / "build", ignore_errors=True)
    shutil.rmtree(src_dir / "bin", ignore_errors=True)
    # cmake
    cmake_cmd = cmake_template.substitute(src_dir=src_dir)
    cmake_result = subprocess.run(cmake_cmd, shell=True)
    # build
    xmake_cmd = xmake_template.substitute(src_dir=src_dir)
    xmake_result = subprocess.run(xmake_cmd, shell=True)
    # results
    assert(not cmake_result.returncode)
    assert(not xmake_result.returncode)

print("build OK")
