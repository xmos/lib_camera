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
imgs = cwd / "src" / "imgs"
test_files = imgs.glob("*.raw")

# cmake, make, run commands
cmake_tmpl = Template(
    'cmake \
        -D FILE_IN_NAME="$file_in" \
        -D FILE_OUT_NAME="$file_out" \
        -G Ninja -B build --fresh --log-level=ERROR'
)
build_cmd = 'ninja -C build'
run_cmd = 'xsim --xscope "-offline trace.xmt" bin/test_isp_rgb1.xe'


def raw_to_rgb_xcore(raw_file: Path):
    # give cmake a input and output file and runs the isp test
    in_cmake = str(raw_file.relative_to(cwd)).replace("\\", "/")
    out_cmake = in_cmake.replace(".raw", ".rgb")
    out_path = raw_file.with_suffix(".rgb")
    cmake_cmd = cmake_tmpl.substitute(file_in=in_cmake, file_out=out_cmake)
    subprocess.run(cmake_cmd, shell=True, cwd=cwd, check=True)
    subprocess.run(build_cmd, shell=True, cwd=cwd, check=True)
    subprocess.run(run_cmd, shell=True, cwd=cwd, check=True)
    # decode rgb to png image
    dec = ImageDecoder(mode="rgb")
    out = raw_file.with_name(raw_file.stem + "_xcore").with_suffix(".png")
    return dec.decode_rgb(out_path, out)


def raw_to_rgb_python(raw_file: Path):
    # takes raw image and convert it to rgb and rgb to png
    dec = ImageDecoder(mode="raw8")
    out = raw_file.with_name(raw_file.stem + "_python").with_suffix(".png")
    return dec.decode_raw8(raw_file, out)


@pytest.mark.parametrize("file_in", test_files)
def test_isp(file_in):
    print("\n===================================")
    print("Testing file:", file_in)
    img_xcore = raw_to_rgb_xcore(file_in)
    img_python = raw_to_rgb_python(file_in)
    met.get_metric(img_xcore, img_python, idx=file_in.stem, check=True, mprint=True)


if __name__ == "__main__":
    pytest.main()
