# load a png or jpg image and encodes to RAW8 format
import os
import cv2
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image  # just to avoid color BGR issues when writting
import sys

# append current path to sys.path
from pathlib import Path
root = (str(Path(__file__).resolve().parent))

# reads an image using pillow
img = Image.open(root+"/test_imgs/test.jpg")
img = img.convert('RGB')
img = img.resize((640//2, 480//2))

# convert to numpy array
img = np.array(img).astype(np.uint8)
raw_img = np.zeros((480, 640), dtype=np.uint8)

# split in color planes
raw_img[0::2, 0::2] = img[:,:,0] 
raw_img[0::2, 1::2] = img[:,:,1] 
raw_img[1::2, 0::2] = img[:,:,1]
raw_img[1::2, 1::2] = img[:,:,2]
raw_img = raw_img.clip(0, 255).astype(np.uint8)

# 1 gamma
raw_img = raw_img/255.0
raw_img = np.power(raw_img, 1.8)
raw_img = raw_img*255.0
raw_img = raw_img.clip(0, 255).astype(np.uint8)

# 2 awb
rgb_gain = [0.5, 0.9, 0.5]
raw_img[0::2, 0::2] = raw_img[0::2, 0::2]*rgb_gain[0]
raw_img[0::2, 1::2] = raw_img[0::2, 1::2]*rgb_gain[1]
raw_img[1::2, 0::2] = raw_img[1::2, 0::2]*rgb_gain[1]
raw_img[1::2, 1::2] = raw_img[1::2, 1::2]*rgb_gain[2]
raw_img = raw_img.clip(0, 255).astype(np.uint8)

# 3 black level substraction
BLACK_LEVEL = 16
raw_img  += BLACK_LEVEL
raw_img  = raw_img.astype(np.uint8)

# save as a binary file
name  = r"/Users/albertoisorna/exec/fwk_camera_windows/test_generated.raw"
with open(name, "wb") as img:
    img.write(raw_img)
    
# save as int8
raw_img  = raw_img.astype(np.int16)
raw_img -= 128
raw_img  = raw_img.astype(np.int8)
name  = r"/Users/albertoisorna/exec/fwk_camera_windows/test_generated_int8.raw"
with open(name, "wb") as img:
    img.write(raw_img)
    
