# Copyright 2023 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import os
import time
import glob

from pathlib import Path
import sys
import cv2
import numpy as np

from matplotlib import pyplot as plt
from PIL import Image

# load dotenv
from dotenv import load_dotenv
load_dotenv()

# path definitions
cwd = Path(__file__).parent.resolve()
top_level = str(cwd.parent.parent.parent.resolve())
examples = top_level + "\\build\\examples"
python_path = top_level + "\\python"
TARGET_NAME = "/take_picture_local/example_take_picture_local.xe"

# python dependencies
sys.path.append(str(python_path))
from encode_raw8 import encode_raw8
from run_xscope_bin import *


def test_pipeline_image():
    # set the test folder directory
    test_imgs_dir = os.getenv("TEST_IMAGES_DIR")
    assert test_imgs_dir is not None

    # 1 - create dir if they dont exist
    dirs = ["input", "raw8", "output"]
    full_paths = [os.path.join(test_imgs_dir, dir) for dir in dirs]

    for p in full_paths:
        Path(p).mkdir(parents=False, exist_ok=True)

    # 2 - Take all input images from input folder
    glob_pattern = os.path.join(test_imgs_dir, "input", "*.png")
    files = glob.glob(glob_pattern)

    # 3 - Encode them to raw8 in the raw8 folder
    out_paths = []

    for file in files:
        img_name = os.path.basename(file).replace(".png", ".raw")
        out_name = os.path.join(test_imgs_dir, "raw8", img_name)
        out_paths.append(out_name)
        encode_raw8(file, out_name)

    # 4 - Then run the pipeline on the raw8 images
    for raw_img in out_paths:
        print("running pipeline on: ", raw_img)
        # copy the image to tmp file
        shutil.copy(raw_img, "tmp.raw")
        # run the inference
        run(examples + TARGET_NAME)
        # Save capture.bmp to output folder
        img_name = os.path.basename(raw_img).replace(".raw", ".bmp")
        shutil.copy("capture.bmp", os.path.join(test_imgs_dir, "output", img_name))
        
        time.sleep(0.2)
