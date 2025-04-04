# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import numpy as np
import os
import pandas as pd
import subprocess
import matplotlib.pyplot as plt

from pathlib import Path
from utils import ImageDecoder, ImageMetrics, ImgSize

RMSE_TOL = 6 # rmse tolerance allowed

# Globals
met = ImageMetrics()
met.rmse_tol = RMSE_TOL
cwd = Path(__file__).parent.absolute()

# Define Paths
imgs = cwd / "imgs"
bin_path = cwd / "bin"
binary = bin_path / "test_isp_wb.xe"
test_files = imgs.glob("*.raw")
test_input_sizes = [64, 32, 58]

assert imgs.exists(), f"Folder {imgs} does not exist"
assert binary.exists(), f"Binary {binary} does not exist"


def run_xcore(file_in: Path, file_out: Path, channels: int, in_size: ImgSize):
    # run command
    run_cmd = [
        "xsim",
        "--xscope",
        "-offline trace.xmt",
        "--args",
        binary.relative_to(cwd),
        str(in_size.height),
        str(in_size.width),
        str(channels),
        file_in.relative_to(cwd),
        file_out.relative_to(cwd),
    ]
    subprocess.run(run_cmd, cwd=cwd, check=True)
    return

@pytest.mark.parametrize("file_in", test_files)
@pytest.mark.parametrize("in_size", test_input_sizes)
def test_swb(file_in, in_size):
    print("\n===================================")
    print("Testing file:", file_in)

    InSize = ImgSize(height=in_size, width=in_size, channels=1, dtype=np.int8)
    OutSize = ImgSize(height=in_size, width=in_size, channels=3, dtype=np.int8)
    InDec = ImageDecoder(InSize)

    # prepare folders
    filename = Path(file_in.name)
    out_folder = imgs / file_in.stem
    out_folder.mkdir(exist_ok=True)
    tmp_in_bin = out_folder / filename.with_suffix(f".python.bin")
    tmp_in_xc = out_folder / filename.with_suffix(f".xcore.bin")

    # convert to rgb, do white abalnce
    img = InDec.raw8_to_rgb1(file_in)
    img = np.array(img, dtype=np.int8)
    img.tofile(tmp_in_bin)

    # run white balance - firmware
    run_xcore(tmp_in_bin, tmp_in_xc, 3, OutSize)
    
    # run white balance python
    # (has to go after firmware)
    img_wb = InDec.rgb_apply_static_wb(img)
    img_wb.tofile(tmp_in_bin)
    
    # plot both arrays
    arr_py = np.fromfile(tmp_in_bin, dtype=np.int8)
    arr_xc = np.fromfile(tmp_in_xc, dtype=np.int8)
    
    # assert metric
    rmse = met.rmse(arr_py, arr_xc)
    assert rmse < met.rmse_tol, f"RMSE is too high: {rmse}"
    print(f"rmse: {rmse}")
    
    
if __name__ == "__main__":
    file_in = list(test_files)[2]
    in_size = 58
    test_swb(file_in, in_size)

   
