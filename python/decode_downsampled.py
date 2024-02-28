# Copyright 2023-2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

"""
Downsampled image info:
    * 160x120
    * R G B
"""
import os
import cv2
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image  # just to avoid color BGR issues when writting
import argparse
import sys

from utils import show_histogram_by_channel


def decode_downsampled_image(input_name="capture.bin", width=160, height=120, img_dtype="uint8"):
    # read the data
    with open(input_name, "rb") as f:
        data = f.read()

    # unpack the data
    if img_dtype == np.uint8:
        buffer = np.frombuffer(data, dtype=np.uint8)
    else:
        buffer = np.frombuffer(data, dtype=np.int8)
        buffer = buffer + 128
        buffer = buffer.astype(np.uint8)

    img = buffer.reshape(height, width, 3)
    print("unpacked_data")

    # show image
    plt.figure()
    plt.imshow(img)
    plt.show()

    # show histograms
    show_histogram_by_channel(img, 3000)


if __name__ == "__main__":
    argparse = argparse.ArgumentParser()
    argparse.add_argument("--input", help="input file name", default="capture.bin")
    argparse.add_argument("--width", help="image width", default=160, type=int)
    argparse.add_argument("--height", help="image height", default=120, type=int)
    argparse.add_argument("--dtype", help="image dtype", default="uint8")
    args = argparse.parse_args()
    decode_downsampled_image(args.input, args.width, args.height, args.dtype)
    print("done")
