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

@pytest.fixture
def print_report():
    yield
    print("\n\n")
    print("=============== Test results =========================")
    for res in test_results: print(res)
    print("======================================================")

@pytest.mark.usefixtures("print_report")
@pytest.mark.parametrize("file_in", test_files)
def test_rgb2(file_in):
    print("\n===================================")
    print("Testing file:", file_in)
    h, w = 200, 200
    dec = ImageDecoder(height=h, width=w, channels=1, dtype=np.int8)
    # run opencv
    outfile = file_in.with_name(file_in.stem + "_opencv").with_suffix(".png")
    img2 = dec.raw8_to_rgb2(file_in, outfile)
    # run xcore (Python)
    outfile = file_in.with_name(file_in.stem + "_python_xcore").with_suffix(".png")
    img1 = dec.raw8_to_rgb2_xcore(file_in, outfile)
    # run xcore (xcore)
    # TODO
    # metrics
    result_opt = met.get_metric(img1, img2, idx=file_in.stem, check=True, mprint=True)
    test_results.append(result_opt)


if __name__ == "__main__":
    pytest.main([__file__])
