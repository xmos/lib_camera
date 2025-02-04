# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import numpy as np
import subprocess
from pathlib import Path

from utils import ImageDecoder, ImageMetrics, ImgSize

met = ImageMetrics()
cwd = Path(__file__).parent.absolute()

# Define test files
imgs = cwd / "imgs"
bin_path = cwd / "bin"
assert imgs.exists(), f"Folder {imgs} does not exist"
test_files = imgs.glob("*.raw")
test_results = []

# Input image configuration
in_size_raw = ImgSize(height=192, width=192, channels=1, dtype=np.int8)

def run_xcore(file_in, file_out, rgb_format):

  tmp_in = imgs / f"in_{rgb_format}.bin"
  tmp_out = imgs / f"out_{rgb_format}.rgb"
  binary = bin_path / f"test_isp_{rgb_format}.xe"

  ds_factor = int(rgb_format[-1])
  out_size = ImgSize(height=192//ds_factor, width=192//ds_factor, channels=3, dtype=np.int8)

  # read and resize an input file to a temp binary file
  dec = ImageDecoder(in_size_raw)
  dec.raw8_resize(file_in, tmp_in, in_size_raw)

  # cmake, make, run commands
  run_cmd = f'xsim --xscope "-offline trace.xmt" {binary}'
  subprocess.run(run_cmd, shell=True, cwd=cwd, check=True)
  
  # decode the output temp image to desired output
  dec = ImageDecoder(out_size)
  img = dec.rgb_to_png(tmp_out, file_out)
  return img

@pytest.mark.parametrize("file_in", test_files)
@pytest.mark.parametrize("rgb_format", ["rgb1", "rgb2", "rgb4"])
def test_rgb(file_in, rgb_format):
  print("\n===================================")
  print("Testing file:", file_in, rgb_format)
  dec = ImageDecoder(in_size_raw)

  # out folder
  out_folder = imgs / file_in.stem
  out_folder.mkdir(exist_ok=True)

  # ------- run opencv
  ref_name = f"{file_in.stem}_{rgb_format}_opencv.png"
  ref_out = out_folder / ref_name
  filter_handle = getattr(dec, f"raw8_to_{rgb_format}")
  ref_img = filter_handle(file_in, ref_out)

  # ------- run xcore (Python)
  py_name = f"{file_in.stem}_{rgb_format}_python.png"
  py_out = out_folder / py_name
  filter_handle = getattr(dec, f"raw8_to_{rgb_format}_xcore")
  py_img = filter_handle(file_in, py_out)

  # ------- Results (opencv vs python)
  results = met.get_metric(ref_name, ref_img, py_name, py_img)
  test_results.append(results)

  # rgb4 is not implemented in C yet
  if rgb_format == "rgb4":
    return

  # ------- run xcore (xcore)
  xc_name = f"{file_in.stem}_{rgb_format}_xcore.png"
  xc_out = out_folder / xc_name
  xc_img = run_xcore(file_in, xc_out, rgb_format)

  # ------- Results (opencv vs xcore)
  results = met.get_metric(ref_name, ref_img, xc_name, xc_img)
  test_results.append(results)
