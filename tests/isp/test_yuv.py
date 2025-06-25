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

RMSE_TOL = 3 # pixel rmse tolerance allowed

# Globals
met = ImageMetrics()
met.rmse_tol = RMSE_TOL
cwd = Path(__file__).parent.absolute()

# Define Paths
imgs = cwd / "imgs"
bin_path = cwd / "bin"
binary = bin_path / "test_isp_yuv.xe"
test_files = imgs.glob("*.raw")
test_input_sizes = [64]

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
def test_yuv(file_in, in_size):
    print("\n===================================")
    print("Testing file:", file_in)

    InSize = ImgSize(height=in_size, width=in_size, channels=1, dtype=np.int8)
    OutSize = ImgSize(height=in_size//2, width=in_size//2, channels=2, dtype=np.int8)
    InDec = ImageDecoder(InSize)
    OutDec = ImageDecoder(OutSize)

    # prepare folders
    out_filename = Path(file_in.stem + f"_{in_size}_yuv")
    out_folder = imgs / file_in.stem
    out_folder.mkdir(exist_ok=True)

    # prepare output files
    file_tmp = out_folder / out_filename.with_suffix(f".tmp.bin")

    file_py_png = out_folder / out_filename.with_suffix(".py.png")
    file_py_bin = out_folder / out_filename.with_suffix(".py.bin")

    file_xc_bin = out_folder / out_filename.with_suffix(f".xc.bin")
    file_xc_png = out_folder / out_filename.with_suffix(".xc.png")

    # prepare xcore input
    InDec.raw8_resize(file_in, file_tmp, InSize)

    # raw to yuv [python]
    OutDec.dtype = np.uint8
    InDec.raw8_to_yuv422(file_in, file_py_bin)
    img_py = OutDec.yuv422_to_rgb_png(file_py_bin, file_py_png)
    
    # raw to yuv [xcore]
    OutDec.dtype = np.int8
    run_xcore(file_tmp, file_xc_bin, 2, InSize)
    img_xc = OutDec.yuv422_to_rgb_png(file_xc_bin, file_xc_png)

    # compare images
    res = met.get_metric(file_py_png, img_py, file_xc_png, img_xc, check=True)
    print(res)
    
if __name__ == "__main__":
    file_in = list(test_files)[0]
    in_size = 128
    test_yuv(file_in, in_size)

   
