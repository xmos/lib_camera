# Copyright 2024-2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

"""
This test is intended to be run on the XK-EVK-XU316-AIV board with the camera module.
This test has to be run manually. 
The test will run the AE and AWB algorithms in different configurations and display the results.
It expects the user to have the camera module connected to the board and the board powered on.
It expects previous compilation of the test binaries.
"""

import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from PIL import Image  # To avoid color BGR issues when writing
import subprocess

cwd = Path(__file__).parent.absolute()
path_imgs = cwd

folder_in = path_imgs
folder_out = path_imgs
assert folder_in.exists(), f"Folder {folder_in} does not exist"

def decode(input_name=None, output_name=None):
    height, width = 200, 200
    buffer = np.fromfile(input_name, dtype=np.int8)
    buffer = buffer.astype(np.int16) + 128 # to convert to uint8
    buffer = buffer.astype(np.uint8)
    img = buffer.reshape(height, width, 3).astype(np.uint8)
    img_pil = Image.fromarray(img)
    img_pil.save(output_name)
    return img_pil

def run_bin(bin_name, input_name, output_name):
    cmd_run = ["xrun", "--xscope", bin_name]
    subprocess.run(cmd_run, check=True)
    img = decode(input_name=input_name, output_name=output_name)
    return img

if __name__ == "__main__":
    input_name = folder_in / "capture1.rgb"
    output_name = folder_out / "capture1.png"
    
    # run ae and awb off
    bin_ae_ae_off = cwd / "bin" / "test_awb_ae_off.xe"
    img_ae_ae_off = run_bin(bin_name=bin_ae_ae_off, input_name=input_name, output_name=output_name)
    
    # run ae off
    bin_ae_off = cwd / "bin" / "test_awb_on_ae_off.xe"
    img_ae_off = run_bin(bin_name=bin_ae_off, input_name=input_name, output_name=output_name)

    # run ae on
    bin_ae_on = cwd / "bin" / "test_awb_ae_on.xe"
    img_ae_on = run_bin(bin_name=bin_ae_on, input_name=input_name, output_name=output_name)
    
    # plot both images
    fig, ax = plt.subplots(1, 3, figsize=(10, 5))
    ax[0].imshow(img_ae_ae_off)
    ax[0].set_title("AE and AWB OFF")
    ax[1].imshow(img_ae_off)
    ax[1].set_title("AWB ON, AE OFF")
    ax[2].imshow(img_ae_on)
    ax[2].set_title("AWB ON, AE ON")
    plt.tight_layout()
    plt.show()
