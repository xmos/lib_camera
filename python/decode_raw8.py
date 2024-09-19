# Copyright 2023 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

"""
Info of RAW streams

SBGGR8:
    640x480 : pixels
    BGGR is the order of the Bayer pattern
    
SBGGR10_CSI2P :
    640x480 stride 800 : bytes per line
    SBGGR10_CSI2P : 10bits per pixel, CSI2 packed format
    BGGR is the order of the Bayer pattern
    few padding bytes on the end of every row to match bits
"""
import os
import cv2
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image  # just to avoid color BGR issues when writting

from utils import (
    normalize,
    simple_white_balance,
    demosaic,
    new_color_correction,
    plot_imgs
)

input_name = os.getenv('BINARY_IMG_PATH') or "capture.raw"

width, height = 640, 480

flip = False

as_shot_neutral = [0.6301882863, 1, 0.6555861831]

flip_mode = {
    0: [0, 1, 1, 2], # R G G B
    1: [1, 0, 2, 1], # G R B G
    2: [1, 2, 0, 1], # G B R G
    3: [2, 1, 1, 0], # B G G R
}

#cfa_pattern = [2, 1, 1, 0] # raspberry
cfa_pattern = [0, 1, 1, 2] # explorer board
cfa_pattern = flip_mode[0]
print("ensure CFA pattern is correct")

# read the data
with open(input_name, "rb") as f:
    data = f.read()


# unpack
#buffer = np.frombuffer(data, dtype=np.int8) + 128 # convert to uint8 
#buffer = buffer.astype(np.uint8)
buffer = np.frombuffer(data, dtype=np.uint8)

img = buffer.reshape(height, width, 1)
print("unpacked_data")

# --> first exit here: save directly bayer data (just remove LSB)
img_raw_bayer = img #>> 2 # remove the first bits
img_raw_bayer_clip = img_raw_bayer.astype(np.uint8)
img_raw_RGB = cv2.cvtColor(img_raw_bayer_clip, cv2.COLOR_BayerBG2RGB) #is normal that raw is not the same as in c
name = f"{input_name}_unprocessesed_.png"
Image.fromarray(img_raw_RGB).save(name) # option 1 pillow
# -->

# ------ The ISP pipeline -------------------------
# black level substraction
img = normalize(img, 15, 254, np.uint8)  
# white balancing
img = simple_white_balance(img, as_shot_neutral, cfa_pattern)
# demosaic
img  = demosaic(img, cfa_pattern, output_channel_order='RGB', alg_type='VNG')
img_demoisaic = img
# color transforms
img = new_color_correction(img)
# gamma
img = img ** (1.0 / 1.8)
# clip the image
img = np.clip(255*img, 0, 255).astype(np.uint8)
# hist equalization (optional)
#   img = run_histogram_equalization(img)
# resize bilinear (optional)
kfactor = 1
img = cv2.resize(img, (width // kfactor, height // kfactor), interpolation=cv2.INTER_AREA)
# ------ The ISP pipeline -------------------------

################# PLOT ##############################

# save image
if flip:
    imgs  = cv2.flip(img, 0)
else:
    imgs = img
    
name = f"{input_name}_postprocess_.png"
Image.fromarray(imgs).save(name) 
print(name)

# plot both images
plot_imgs(img, img_raw_RGB, flip)
plt.show()

# calculus of color contribution
red = int(img_demoisaic[:,:,0].sum())
green = int(img_demoisaic[:,:,1].sum())
blue = int(img_demoisaic[:,:,2].sum())

txt = f"blue: {blue}, green: {green}, red: {red}"
print(txt)
