import time
import os

import cv2
from matplotlib import pyplot as plt
import numpy as np

from utils import peak_signal_noise_ratio
from PIL import Image
from run_xscope_bin import *
from pathlib import Path

from FIR_pipeline import FIR_pipeline
from utils import pipeline


# path definitions
cwd = str(Path(__file__).parent.resolve())
top_level = str(Path(__file__).parent.parent.resolve())
examples  = top_level + "/build/examples"


def load_image(input_name, height, width, ch=0):
    with open(input_name, "rb") as f:
        data = f.read()
        buffer = np.frombuffer(data, dtype=np.uint8)
        if ch == 0:
            buffer = buffer.reshape(height, width)
        else:
            buffer = buffer.reshape(height, width, ch)
    return buffer

if __name__ == '__main__':
    ENABLE_IMSHOW = 1
    kfactor = 4
    
    run(examples + '/take_picture_raw/example_take_picture_raw.xe')
    time.sleep(1)
    run(examples + '/take_picture_downsample/example_take_picture_downsample.xe')
    time.sleep(1)
    
    # load
    width, height, input_name = 640, 480, 'capture.raw'
    img_raw = load_image(input_name, height, width)
    
    # highest quality
    ref_image = pipeline(img_raw, False)
    ref_image = cv2.resize(ref_image, (width // kfactor, height // kfactor), interpolation=cv2.INTER_AREA)
    
    # raw with python pipeline
    img_raw_FIR = FIR_pipeline(img_raw, height, width, ENABLE_IMSHOW)

    # downsampled
    input_name_dwn = 'capture.bin'
    width, height = 160, 120
    img_dwn = load_image(input_name_dwn, height, width, 3)

    # show the 3 images
    plt.subplot(1, 3, 1)
    plt.imshow(ref_image)
    plt.subplot(1, 3, 2)
    plt.imshow(img_raw_FIR)
    plt.subplot(1, 3, 3)
    plt.imshow(img_dwn)
    
    # PSNR
    img_psnr = peak_signal_noise_ratio(ref_image, img_raw_FIR) 
    print(img_psnr)
        
