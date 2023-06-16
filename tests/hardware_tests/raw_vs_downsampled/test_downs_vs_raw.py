import os
import time
from pathlib import Path
import sys
import cv2
import numpy as np

from matplotlib import pyplot as plt
from PIL import Image

# path definitions
cwd = Path(__file__).parent.resolve()
top_level   = str(cwd.parent.parent.parent.resolve())
examples    = top_level + "\\build\\examples"
python_path = top_level + "\\python"

# python dependencies
sys.path.append(str(python_path))
from FIR_pipeline import FIR_pipeline_func
from utils import peak_signal_noise_ratio, pipeline_raw8
from run_xscope_bin import *

def load_image(input_name, height, width, ch=0):
    buffer = np.fromfile(input_name, dtype=np.uint8)
    shape = (height, width) if ch == 0 else (height, width, ch)
    buffer = buffer.reshape(shape)
    return buffer

if __name__ == '__main__':
    ENABLE_IMSHOW   = True
    RUN_XE          = False
    
    if RUN_XE:
        run(examples + '/take_picture_raw/example_take_picture_raw.xe')
        time.sleep(1)
        run(examples + '/take_picture_downsample/example_take_picture_downsample.xe')
        time.sleep(1)
    
    # base image raw 640x480
    width, height, input_name = 640, 480, 'capture.raw'
    img_raw = load_image(input_name, height, width)
    
    # 1 - raw with decode 8 pipeline
    ref_image = pipeline_raw8(img_raw)
    ref_image = cv2.resize(ref_image, (width // 4, height // 4), interpolation=cv2.INTER_AREA)
    
    # 2 - raw with simplified FIR pipeline
    img_raw_FIR = FIR_pipeline_func(img_raw, height, width, ENABLE_IMSHOW)

    # 3 - downsampled image in the explorer board
    input_name_dwn = 'capture.bin'
    width, height = 160, 120
    img_dwn = load_image(input_name_dwn, height, width, 3)

    ##############################################
    imgs = [ref_image, img_raw_FIR, img_dwn]
    if ENABLE_IMSHOW:
        # Show the 3 imageslist(
        fig, axes = plt.subplots(2, 3)
        titles = [
            "RAW decode8",
            "RAW python FIR",
            "Downsampled\nXcore pipeline"
        ]
        def add_caption(ax, caption):
            axes[0, i].text(0, -0.1, caption, transform=ax.transAxes,
                        ha='left', va='top', fontsize=8)
            
        for i, title in enumerate(titles):
            axes[0,i].set_title(title)
            axes[0,i].imshow(imgs[i])
            axes[1,i].set_ylim([0, 3000])
            # histogram
            for j, col in enumerate(['r', 'g', 'b']):
                axes[1,i].hist(imgs[i][:,:,j].ravel(), bins=256, alpha=0.5, color=col)

            # text
            img_psnr = peak_signal_noise_ratio(ref_image, imgs[i]) 
            score = 0
            text = f"SSI: {score:.2f}, \nPSNR: {img_psnr:.2f}"
            add_caption(axes[0,i], text)
            
        plt.subplots_adjust(wspace=0.05)
        list(map(lambda axi: axi.set_axis_off(), axes.ravel()[0:3]))
        plt.show()
            
