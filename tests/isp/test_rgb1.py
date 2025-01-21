# Copyright 2024-2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import numpy as np
import subprocess
from pathlib import Path

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
        -G Ninja -B build --fresh --log-level=ERROR'
)
build_cmd = "ninja -C build"
run_cmd = 'xsim --xscope "-offline trace.xmt" bin/test_isp_rgb1.xe'


def run_raw8_to_rgb1_xcore(raw_file: Path, outfile: Path):
    # give cmake a input and output file and runs the isp test
    in_cmake = str(raw_file.relative_to(cwd)).replace("\\", "/")
    out_cmake = in_cmake.replace(".raw", ".rgb")
    out_path = raw_file.with_suffix(".rgb")
    cmake_cmd = cmake_tmpl.substitute(file_in=in_cmake, file_out=out_cmake)
    subprocess.run(cmake_cmd, shell=True, cwd=cwd, check=True)
    subprocess.run(build_cmd, shell=True, cwd=cwd, check=True)
    subprocess.run(run_cmd, shell=True, cwd=cwd, check=True)
    dec = ImageDecoder(mode="rgb")  # decode rgb to png image
    return dec.decode_rgb(out_path, outfile)


@pytest.mark.parametrize("file_in", test_files)
def test_rgb1(file_in):
    print("\n===================================")
    print("Testing file:", file_in)
    h, w = 200, 200
    dec = ImageDecoder(height=h, width=w, channels=1, dtype=np.int8)

    # ------- run opencv
    ref_name = file_in.stem + "_rgb1_opencv"
    ref_out = file_in.with_name(ref_name).with_suffix(".png")
    ref_img = dec.raw8_to_rgb1(file_in, ref_out)

    # ------- run xcore (Python)
    py_name = file_in.stem + "_rgb1_python"
    py_out = file_in.with_name(py_name).with_suffix(".png")
    py_img = None  # TODO implement RGB2 py xcore

    # ------- run xcore (xcore)
    xc_name = file_in.stem + "_rgb1_xcore"
    xc_out = file_in.with_name(xc_name).with_suffix(".png")
    xc_img = run_raw8_to_rgb1_xcore(file_in, xc_out)

    # ------- Results (opencv vs python)
    # TODO implement py_xcore
    # results = met.get_metric(ref_name, ref_img, py_name, py_img)
    # test_results.append(results)
    
    # ------- Results (opencv vs xcore)
    results = met.get_metric(ref_name, ref_img, xc_name, xc_img)
    test_results.append(results)


@pytest.fixture(scope="session", autouse=True)
def print_results_at_end(request):
    """Fixture to print results at the end of the test session."""

    def print_results():
        print("\n\nFinal Test Results:")
        for result in test_results:
            print(result)

    request.addfinalizer(print_results)


if __name__ == "__main__":
    pytest.main()
