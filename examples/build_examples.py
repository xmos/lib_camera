# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

# This file builds all examples in the examples folder within a single script.

import shutil
import subprocess
from string import Template 

generator = "Ninja" # or "Unix Makefiles"
make = "ninja" # or "xmake"

cmake_template = Template(f'cmake -G {generator} -S $src_dir -B $src_dir/build')
xmake_template = Template(f'{make} -C $src_dir/build')


# List of folders
dirs_srcs = [
    "take_picture_downsample",
    "take_picture_local",
    "take_picture_raw"
]

# Store result codes
result_codes = []

# Run each command
for dir in dirs_srcs:
    # remove build and dir folders
    shutil.rmtree(dir + "/build", ignore_errors=True)
    shutil.rmtree(dir + "/bin", ignore_errors=True)
    # cmake
    cmake_cmd = cmake_template.substitute(src_dir=dir)
    cmake_result = subprocess.run(cmake_cmd, shell=True)
    # build
    xmake_cmd = xmake_template.substitute(src_dir=dir)
    xmake_result = subprocess.run(xmake_cmd, shell=True)
    # results
    assert(not cmake_result.returncode)
    assert(not xmake_result.returncode)
    result_codes.append(cmake_result.returncode)
    result_codes.append(xmake_result.returncode)

# Print result codes
print("Result codes:", result_codes)
