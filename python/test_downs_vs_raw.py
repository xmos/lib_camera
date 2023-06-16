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
    ENABLE_IMSHOW   = False
    RUN_XE          = True
    kfactor         = 4
    
    if RUN_XE:
        run(examples + '/take_picture_raw/example_take_picture_raw.xe')
        time.sleep(1)
        run(examples + '/take_picture_downsample/example_take_picture_downsample.xe')
        time.sleep(1)
    
    # base image raw 640x480
    width, height, input_name = 640, 480, 'capture.raw'
    img_raw = load_image(input_name, height, width)
    
    # 1 - raw with decode 8 pipeline
    ref_image = pipeline(img_raw, False)
    ref_image = cv2.resize(ref_image, (width // kfactor, height // kfactor), interpolation=cv2.INTER_AREA)
    
    # 2 - raw with simplified FIR pipeline
    img_raw_FIR = FIR_pipeline(img_raw, height, width, ENABLE_IMSHOW)

    # 3 - downsampled image in the explorer board
    input_name_dwn = 'capture.bin'
    width, height = 160, 120
    img_dwn = load_image(input_name_dwn, height, width, 3)

    ##############################################
    # Show the 3 imageslist(
    fig, axes = plt.subplots(2, 3)
    titles = [
        "RAW decode8",
        "RAW python FIR",
        "Downsampled\nXcore pipeline"
    ]
    imgs = [ref_image, img_raw_FIR, img_dwn]
    
    for i, title in enumerate(titles):
        axes[0,i].set_title(title)
        axes[0,i].imshow(imgs[i])
        axes[1,i].set_ylim([0, 3000])
        axes[1,i].hist(imgs[i].ravel(), bins=256, color='gray', alpha=0.5)
        

    plt.subplots_adjust(wspace=0.05)
    list(map(lambda axi: axi.set_axis_off(), axes.ravel()[0:3]))
    plt.show()
    # PSNR
    img_psnr = peak_signal_noise_ratio(ref_image, img_raw_FIR) 
    print(img_psnr)
        
