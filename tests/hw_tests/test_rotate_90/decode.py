# Copyright 2023-2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

cwd = Path(__file__).parent.absolute()
imgs_path = cwd.parent / "imgs"

def decode_downsampled_image(
    input_name="capture.bin", width=160, height=120, img_dtype="uint8", plot=False
):
    print("Decoding image : ", input_name.name)

    # read the data
    with open(input_name, "rb") as f:
        data = f.read()

    # unpack the data
    if img_dtype == "uint8":
        buffer = np.frombuffer(data, dtype=np.uint8)
    elif img_dtype == "int8":
        buffer = np.frombuffer(data, dtype=np.int8)
        buffer = buffer + 128
        buffer = buffer.astype(np.uint8)
    else:
        raise ValueError(f"Invalid dtype: {img_dtype}")

    img = buffer.reshape(height, width, 3)

    if plot:
        # show image
        plt.figure()
        plt.imshow(img)
        plt.show()
    return img


if __name__ == "__main__":
    # The list of images to decode
    # All the images are listed as couples of input and output images
    imgs = [
        imgs_path / "input_rgb_uint8_64_64.bin",
        imgs_path / "output_rgb_uint8_64_64.bin",
        imgs_path / "input_rgb_uint8_64_64.bin",
        imgs_path / "output_rgb_uint8_64_64.bin",
    ]

    sizes = [(64, 64), (64, 64), (64, 64), (64, 64)]
    titles = ["Original - uint8", "Rotated - uint8", "Original - int8", "Rotated - int8"]

    # Decode the images
    imgs_decoded = map(
        lambda x: decode_downsampled_image(x[0], x[1][0], x[1][1]), zip(imgs, sizes)
    )

    # plot image sside by side
    plt.figure()
    for i, img in enumerate(imgs_decoded):
        # Plot one couple of input and output images per row
        plt.subplot(len(imgs) // 2, 2, i + 1)
        plt.title(titles[i])
        plt.imshow(img, cmap="gray")
    plt.savefig(str(imgs_path / "test_rotate_out.png"))
    plt.show()
