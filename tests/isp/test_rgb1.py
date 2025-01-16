# Copyright 2024-2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import matplotlib.pyplot as plt
import numpy as np
import subprocess
from pathlib import Path
from PIL import Image  # To avoid color BGR issues when writing

from string import Template
from utils import ImageDecoder, ImageMetrics

met = ImageMetrics()
cwd = Path(__file__).parent.absolute()

# Define test files
imgs = cwd / "src" / "imgs"
test_files = imgs.glob("*.raw")
test_results = []

# cmake, make, run commands
cmake_tmpl = Template(
    'cmake \
        -D FILE_IN_NAME="$file_in" \
        -D FILE_OUT_NAME="$file_out" \
        -D USE_OPTIMISED=$opt \
        -G Ninja -B build --fresh --log-level=ERROR'
)
build_cmd = "ninja -C build"
run_cmd = 'xsim --xscope "-offline trace.xmt" bin/test_isp_rgb1.xe'


def run_raw_to_rgb1_xcore(raw_file: Path, optimised=0):
    # give cmake a input and output file and runs the isp test
    in_cmake = str(raw_file.relative_to(cwd)).replace("\\", "/")
    out_cmake = in_cmake.replace(".raw", ".rgb")
    out_path = raw_file.with_suffix(".rgb")
    cmake_cmd = cmake_tmpl.substitute(
        file_in=in_cmake, file_out=out_cmake, opt=optimised
    )
    subprocess.run(cmake_cmd, shell=True, cwd=cwd, check=True)
    subprocess.run(build_cmd, shell=True, cwd=cwd, check=True)
    subprocess.run(run_cmd, shell=True, cwd=cwd, check=True)
    # decode rgb to png image
    dec = ImageDecoder(mode="rgb")
    out = raw_file.with_name(raw_file.stem + "_xcore" + f"opt_{optimised}").with_suffix(".png")
    return dec.decode_rgb(out_path, out)


def run_raw_to_rgb_python(raw_file: Path):
    # takes raw image and convert it to rgb and rgb to png
    dec = ImageDecoder(mode="raw8")
    out = raw_file.with_name(raw_file.stem + "_python").with_suffix(".png")
    return dec.raw8_to_rgb1(raw_file, out)

@pytest.fixture
def print_report():
    yield
    print("\n=============== Test results ====================")
    for res in test_results: print(res)
    print("===================================================")
    
@pytest.mark.usefixtures("print_report")
@pytest.mark.parametrize("file_in", test_files)
def test_rgb1(file_in):
    print("\n===================================")
    print("Testing file:", file_in)
    img_xcore_inline = run_raw_to_rgb1_xcore(file_in, 0)
    img_xcore = run_raw_to_rgb1_xcore(file_in, 1)
    img_python = run_raw_to_rgb_python(file_in)
    result_base = met.get_metric(img_xcore_inline, img_python, idx=file_in.stem, check=True, mprint=True)
    result_opt = met.get_metric(img_xcore, img_python, idx=file_in.stem, check=True, mprint=True)
    test_results.append(f"{file_in.name}: non-opt:{result_base} opt:{result_opt}")
    
if __name__ == "__main__":
    pytest.main()
