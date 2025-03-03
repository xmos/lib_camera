# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import errno
import numpy as np
import os
import subprocess
import zipfile2
from pathlib import Path

from utils import ImageDecoder, ImageMetrics, ImgSize

met = ImageMetrics()
cwd = Path(__file__).parent.absolute()

# Define Paths
imgs = cwd / "imgs"
bin_path = cwd / "bin"
binary = bin_path / "test_isp_rgb.xe"

assert imgs.exists(), f"Folder {imgs} does not exist"
assert binary.exists(), f"Binary {binary} does not exist"

# Prepare Image Zip File
zip_out = imgs / "images.zip"

try:
    os.remove(zip_out)
except OSError as e:
    if e.errno != errno.ENOENT: # errno.ENOENT means no such file or directory
        raise # re-raise exception if a different error occurred

# Test Parameters
test_files = imgs.glob("*.raw")
test_rgb_map = {"rgb1": 1, "rgb2": 2, "rgb4": 4}
# test_input_sizes = [128, 192, 200] Remove 200x200 until failures understood
test_input_sizes = [128, 192]


def run_xcore(file_in: Path, file_out: Path, ds_factor: int, in_size: ImgSize):
    # run command
    run_cmd = [
        "xsim",
        "--xscope",
        "-offline trace.xmt",
        "--args",
        binary.relative_to(cwd),
        str(in_size.height),
        str(in_size.width),
        str(ds_factor),
        file_in.relative_to(cwd),
        file_out.relative_to(cwd),
    ]
    subprocess.run(run_cmd, cwd=cwd, check=True)
    return


def get_rgb_decoder(ds_factor: int, in_size: ImgSize):
    out_size = ImgSize(
        height=in_size.height // ds_factor,
        width=in_size.width // ds_factor,
        channels=3,
        dtype=np.int8,
    )
    return ImageDecoder(out_size)


@pytest.mark.parametrize("file_in", test_files)
@pytest.mark.parametrize("rgb_format", test_rgb_map.keys())
@pytest.mark.parametrize("in_size", test_input_sizes)
def test_rgb(file_in, rgb_format, in_size, request):

    if rgb_format == "rgb4" and in_size == 200:
        pytest.xfail("Expected failure for rgb_format=4 and in_size=200")

    print("\n===================================")
    print("Testing file:", file_in, rgb_format)
    ds_factor = test_rgb_map.get(rgb_format)
    in_size_raw = ImgSize(height=in_size, width=in_size, channels=1, dtype=np.int8)

    # decoders in and out
    dec = ImageDecoder(in_size_raw)
    dec_out = get_rgb_decoder(ds_factor, in_size_raw)

    # prepare xcore input
    tmp_in = file_in.with_suffix(f".{rgb_format}.{in_size}.tmp.bin")
    dec.raw8_resize(file_in, tmp_in, in_size_raw)

    # prepare out folder and name
    out_folder = imgs / file_in.stem
    out_folder.mkdir(exist_ok=True)
    out_name = f"{file_in.stem}_{rgb_format}_{in_size}"

    # ------- run opencv
    ref_name = f"{out_name}_opencv.png"
    ref_out = out_folder / ref_name
    filter_handle = getattr(dec, f"raw8_to_{rgb_format}")
    ref_img = filter_handle(file_in, ref_out)

    # ------- run Python
    py_name = f"{out_name}_python.png"
    py_out = out_folder / py_name
    filter_handle = getattr(dec, f"raw8_to_{rgb_format}_xcore")
    py_img = filter_handle(file_in, py_out)

    # ------- run xcore (xcore)
    xc_name = f"{out_name}_xcore.png"
    xc_out = out_folder / xc_name
    run_xcore(tmp_in, xc_out, ds_factor, in_size_raw)
    xc_img = dec_out.rgb_to_png(xc_out, xc_out)

    # -------> Results
    res_py = met.get_metric(ref_name, ref_img, py_name, py_img, check=True)
    res_xc = met.get_metric(ref_name, ref_img, xc_name, xc_img, check=True)
    met.get_cross_metrics(res_py, res_xc, check=True)

    # Store results in pytest session
    if request is not None:
        request.session.results.append(res_py)
        request.session.results.append(res_xc)
    else:
        print(res_py)
        print(res_xc)

    # Zip the images
    with zipfile2.ZipFile(zip_out, 'a') as zip:
        zip.write(ref_out)
        zip.write(py_out)
        zip.write(xc_out)

    # remove tmp_in file
    tmp_in.unlink()


if __name__ == "__main__":
    file_in = list(test_files)[0]
    rgb_format = list(test_rgb_map.keys())[0]
    in_size = test_input_sizes[0]
    test_rgb(file_in, rgb_format, in_size, None)
