# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import numpy as np
import cv2
from PIL import Image

def yuv422_to_rgb_and_save(img_path, width, height, output_file):
    buffer = np.fromfile(img_path, dtype=np.int8)
    # to uint8 
    buffer = buffer.astype(np.int16) + 128
    buffer = buffer.astype(np.uint8)
    arr = buffer.reshape((height, width, 2))
    yuv_image = cv2.cvtColor(arr, cv2.COLOR_YUV2RGB_YUY2)
    # to pillow
    pil_image = Image.fromarray(yuv_image)
    pil_image.save(output_file)
    pil_image.show()


if __name__ == "__main__":
    # Example usage
    width = 128
    height = 128
    yuv_file = "capture_yuv.bin"
    output_file = "output_yuv.png"
    
    # Convert YUV to RGB and save the image
    yuv422_to_rgb_and_save(yuv_file, width, height, output_file)


#
