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

def run_raw8_to_rgb2_xcore(raw_file: Path, outfile: Path):
    # TODO implement xcore
    pass


@pytest.mark.parametrize("file_in", test_files)
def test_rgb2(file_in):
    print("\n===================================")
    print("Testing file:", file_in)
    h, w = 200, 200
    dec = ImageDecoder(height=h, width=w, channels=1, dtype=np.int8)
    
    # ------- run opencv
    ref_name = file_in.stem + "_rgb2_opencv"
    ref_out = file_in.with_name(ref_name).with_suffix(".png")
    ref_img = dec.raw8_to_rgb2(file_in, ref_out)
    
    # ------- run xcore (Python)
    py_name = file_in.stem + "_rgb2_python"
    py_out = file_in.with_name(py_name).with_suffix(".png")
    py_img = dec.raw8_to_rgb2_xcore(file_in, py_out)
    
    # ------- run xcore (xcore)
    xc_name = file_in.stem + "_rgb2_xcore"
    xc_out = file_in.with_name(xc_name).with_suffix(".png")
    xc_img = None  # TODO implement xcore

    # ------- Results (opencv vs python)
    results = met.get_metric(ref_name, ref_img, py_name, py_img)
    test_results.append(results)

    # ------- Results (python vs xcore) 
    # TODO implement xcore
    # results = met.get_metric(py_name, img_py, xc_name, xc_img)
    # test_results.append(results)

@pytest.fixture(scope="session", autouse=True)
def print_results_at_end(request):
    """Fixture to print results at the end of the test session."""
    def print_results():
        print("\n\nFinal Test Results:")
        for result in test_results:
            print(result)
    request.addfinalizer(print_results)

        
if __name__ == "__main__":
    pytest.main(["-s", __file__])
