# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import os
import cv2
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image  # just to avoid color BGR issues when writting
from dotenv import load_dotenv
import sys
from pathlib import Path

# import utils
root = Path(__file__).parents[2].absolute()
python_path = root / "python"
sys.path.append(str(python_path))

from utils import (
    normalize,
    simple_white_balance,
    demosaic,
    new_color_correction,
    plot_imgs
)

def process_image(input_name, width=400, height=300, flip=False, as_shot_neutral=[0.6301882863, 1, 0.6555861831]):
    flip_mode = {
        0: [0, 1, 1, 2], # R G G B
        1: [1, 0, 2, 1], # G R B G
        2: [1, 2, 0, 1], # G B R G
        3: [2, 1, 1, 0], # B G G R
    }

    cfa_pattern = flip_mode[0]
    print("ensure CFA pattern is correct")

    # read the data
    with open(input_name, "rb") as f:
        data = f.read()

    buffer = np.frombuffer(data, dtype=np.uint8)

    img = buffer.reshape(height, width, 1)
    print("unpacked_data")

    img_raw_bayer = img
    img_raw_bayer_clip = img_raw_bayer.astype(np.uint8)
    img_raw_RGB = cv2.cvtColor(img_raw_bayer_clip, cv2.COLOR_BayerBG2RGB)
    name = f"{input_name}_unprocessesed_.png"
    Image.fromarray(img_raw_RGB).save(name)

    img = normalize(img, 15, 254, np.uint8)  
    img = simple_white_balance(img, as_shot_neutral, cfa_pattern)
    img  = demosaic(img, cfa_pattern, output_channel_order='RGB', alg_type='VNG')
    img_demoisaic = img
    img = new_color_correction(img)
    img = img ** (1.0 / 1.8)
    img = np.clip(255*img, 0, 255).astype(np.uint8)
    kfactor = 1
    img = cv2.resize(img, (width // kfactor, height // kfactor), interpolation=cv2.INTER_AREA)

    if flip:
        imgs  = cv2.flip(img, 0)
    else:
        imgs = img

    name = f"{input_name}_postprocess_.png"
    Image.fromarray(imgs).save(name) 
    print(name)
    return imgs

def main():
    input_name = "capture1.raw"
    img1 = process_image(input_name, width=200, height=200)
    
    input_name = "capture2.raw"
    img2 = process_image(input_name, width=200, height=200)

    plt.figure(figsize=(10, 10))
    
    plt.subplot(1, 2, 1)
    plt.title("Capture Region 1")
    plt.imshow(img1)
    plt.subplot(1, 2, 2)
    plt.title("Capture Region 2")
    plt.imshow(img2)
    
    plt.show()

if __name__ == "__main__":
    main()
