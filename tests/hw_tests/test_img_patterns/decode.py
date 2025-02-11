# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import numpy as np
from pathlib import Path
from PIL import Image
import matplotlib.pyplot as plt

cwd = Path(__file__).parent
height, width = 480, 640

def split_planes(img):
    red = img[0::2, 0::2]
    green1 = img[0::2, 1::2]
    green2 = img[1::2, 0::2]
    green = (0.5 * green1 + 0.5 * green2).clip(0, 255).astype(np.uint8)
    blue = img[1::2, 1::2]
    return np.stack((red, green, blue)).astype(np.uint8)


def combine_planes(img):  # crude demosaic
    channels = split_planes(img)
    rgb = np.stack(channels, axis=-1)  # Shape: (h, w, 3)
    return rgb.squeeze().astype(np.uint8)  # Removes singleton dimensions


def decode_img(in_path : Path, out_path : Path) -> Image:
    # read data
    with open(in_path, "rb") as f:
        data = f.read()
    buffer = np.frombuffer(data, dtype=np.int8)
    img = buffer.reshape(height, width, 1)

    # to uint8
    img = img.astype(np.int16)
    img = img + 128
    img = np.clip(img, 0, 255).astype(np.uint8)
    img = combine_planes(img)
    img_pil = Image.fromarray(img)
    img_pil.save(out_path)
    print(f"Saved {out_path}")
    return img_pil


if __name__ == "__main__":
    images = []
    for i in range(10):
        input_name = cwd / f"capture_tp_{i}.raw"
        output_name = cwd / f"{input_name.stem}.rgb.png"
        img = decode_img(input_name, output_name)
        images.append(img)
        
    # plot them in a grid of 2x5
    fig, axs = plt.subplots(2, 5)
    for i, ax in enumerate(axs.flat):
        ax.imshow(images[i])
        ax.axis("off")
    plt.show()
