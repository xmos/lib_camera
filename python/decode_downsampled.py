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
from dotenv import load_dotenv
load_dotenv()  # take environment variables from .env.

from utils import show_histogram_by_channel

input_name = os.getenv('BINARY_IMG_PATH') or "capture.bin"

# read the data
with open(input_name, "rb") as f:
    data = f.read()

# unpack
width, height = 160, 120
buffer = np.frombuffer(data, dtype=np.uint8)
img = buffer.reshape(height, width, 3)
print("unpacked_data")

# show image
plt.figure()
plt.imshow(img)
plt.show()

# show histograms
show_histogram_by_channel(img, 3000)
