# Copyright 2023-2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import argparse
import numpy as np
from PIL import Image


def encode_downsampled_image(input_name=None, width=160, height=120, img_dtype="uint8"):
    # this takes an image in png, jpg, etc using pillow
    # then it propduce a binary file with the image
    # the image size is width x height
    # the image is in RGB format
    # the image is in img_dtype format

    img = Image.open(input_name)
    img = img.resize((width, height))
    img = np.array(img)

    if img_dtype == "uint8":
        buffer = img.tobytes()
    elif img_dtype == "int8":
        buffer = img.astype(np.int32) - 128  # avoid overflow
        buffer = buffer.astype(np.int8)
        buffer = buffer.tobytes()
    else:
        raise ValueError("dtype not supported")

    output_name = input_name.split(".")[0] + img_dtype + ".bin"
    with open(output_name, "wb") as f:
        f.write(buffer)
        
    # print a complete message
    print(f"Image {input_name} was downsampled to {output_name} with size {width}x{height} and dtype {img_dtype}")
    

if __name__ == "__main__":
    argparse = argparse.ArgumentParser()
    argparse.add_argument("--input", help="input file name", default="capture.bin")
    argparse.add_argument("--width", help="image width", default=160, type=int)
    argparse.add_argument("--height", help="image height", default=120, type=int)
    argparse.add_argument("--dtype", help="image dtype", default="uint8")
    args = argparse.parse_args()
    encode_downsampled_image(args.input, args.width, args.height, args.dtype)
    print("done")
