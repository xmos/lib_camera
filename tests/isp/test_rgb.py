# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import numpy as np
import subprocess
from pathlib import Path

from utils import ImageDecoder, ImageMetrics, ImgSize

met = ImageMetrics()
cwd = Path(__file__).parent.absolute()

# rgb format map
# rgb_formats_map = {"rgb1": 1, "rgb2": 2, "rgb4": 4} #TODO add rgb4 when is done
rgb_formats_map = {"rgb1": 1, "rgb2": 2}

# Define test files
imgs = cwd / "imgs"
bin_path = cwd / "bin"
binary = bin_path / f"test_isp_rgb.xe"
test_files = imgs.glob("*.raw")
test_results = []

assert imgs.exists(), f"Folder {imgs} does not exist"
assert binary.exists(), f"Binary {binary} does not exist"

# Input image configuration
in_size_raw = ImgSize(height=192, width=192, channels=1, dtype=np.int8)

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
@pytest.mark.parametrize("rgb_format", rgb_formats_map.keys())
def test_rgb(file_in, rgb_format):
    print("\n===================================")
    print("Testing file:", file_in, rgb_format)
    ds_factor = rgb_formats_map.get(rgb_format)
    dec = ImageDecoder(in_size_raw)
    dec_out = get_rgb_decoder(ds_factor, in_size_raw)

    # prepare xcore input
    tmp_in = imgs / f"in_{rgb_format}.bin"
    dec.raw8_resize(file_in, tmp_in, in_size_raw)

    # prepare out folder
    out_folder = imgs / file_in.stem
    out_folder.mkdir(exist_ok=True)

    # ------- run opencv
    ref_name = f"{file_in.stem}_{rgb_format}_opencv.png"
    ref_out = out_folder / ref_name
    filter_handle = getattr(dec, f"raw8_to_{rgb_format}")
    ref_img = filter_handle(file_in, ref_out)

    # ------- run Python
    py_name = f"{file_in.stem}_{rgb_format}_python.png"
    py_out = out_folder / py_name
    filter_handle = getattr(dec, f"raw8_to_{rgb_format}_xcore")
    py_img = filter_handle(file_in, py_out)

    # ------- run xcore (xcore)
    xc_name = f"{file_in.stem}_{rgb_format}_xcore.png"
    xc_out = out_folder / xc_name
    run_xcore(tmp_in, xc_out, ds_factor, in_size_raw)
    xc_img = dec_out.rgb_to_png(xc_out, xc_out)

    # -------> Results (opencv vs python)
    results = met.get_metric(ref_name, ref_img, py_name, py_img)
    test_results.append(results)

    # -------> Results (opencv vs xcore)
    results = met.get_metric(ref_name, ref_img, xc_name, xc_img)
    test_results.append(results)


@pytest.fixture(scope="session", autouse=True)
def print_results_at_end(request):
    """Fixture to print results at the end of the test session."""

    def print_results():
        print("\n\nAll Tests Results:")
        for result in test_results:
            print(result)

    request.addfinalizer(print_results)


if __name__ == "__main__":
    pytest.main([__file__])
