# Raw stream: 640x480 stride 800 format SBGGR10_CSI2P
"""
that means :
    640x480 stride 800 : bytes per line
    SBGGR10_CSI2P : 10bits per pixel, CSI2 packed format
    BGGR is the order of the Bayer pattern
    few padding bytes on the end of every row to match bits
"""
import matplotlib.pyplot as plt
import cv2
import numpy as np
from PIL import Image # just to avoid color BGR issues when writting
from utils import *

image_name = "img_raw10.bin"
path = "/home/albertoisorna/xalbertoisorna/rawtreatment/newimgs/"
path='/mnt/c/Users/albertoisorna/exec/'

image_name = "img_640_480_10_0008.raw"
path = "/home/albertoisorna/xalbertoisorna/rawtreatment/images_binning/adquisition_2_raw/"

input_name = path+image_name
width = 640
height = 480
bit_width = 10
flip = False
as_shot_neutral = [0.5666090846, 1, 0.7082979679]
as_shot_neutral = [0.6301882863, 1, 0.6555861831]
#cfa_pattern = [0, 1, 1, 2] 
cfa_pattern = [2, 1, 1, 0]
 
# read the data
with open(input_name, "rb") as f:
    data = f.read()
    
# unpack
buffer = np.frombuffer(data, dtype=np.uint8)
buffer_nopad =  remove_padding_buffer_no_align(buffer, width, height, bit_width)
unpacked_data = unpack_mipi_raw10_buffer(buffer_nopad)
shaped_data = unpacked_data.reshape(height, width, 1) # dtype=uint16
img = shaped_data
print("unpacked_data")

# --> first exit here: save directly bayer data (just remove LSB)
img_raw_bayer = img >> 2 # remove the first bits
img_raw_bayer_clip = img_raw_bayer.astype(np.uint8)
img_raw_RGB = cv2.cvtColor(img_raw_bayer_clip, cv2.COLOR_BayerRG2RGB)
name = f"{input_name}_unprocessesed_.png"
Image.fromarray(img_raw_RGB).save(name) # option 1 pillow
# cv2.imwrite(name, img_raw_RGB) #TODO opencv expect BGR here

# black level substraction
img = old_normalize(img, 64, 1023) 
    
# white balancing
img = white_balance(img, as_shot_neutral, cfa_pattern)

# demosaic
img  = demosaic(img, cfa_pattern, output_channel_order='RGB', alg_type='VNG')
img_demoisaic = img

# color space transformation
color_matrix_1 = [0.9762914777, -0.2504389584, -0.1018426344, 
                  -0.1751390547, 0.9807397723, 0.1705771685, 
                  0.04482413828, 0.1344814152, 0.4878755212]

#img = old_apply_color_space_transform(img, color_matrix_1)

# color transform 2
# img = old_transform_xyz_to_srgb(img)

# gamma
img = img ** (1.0 / 2.2)

# clip the image
img = np.clip(255*img, 0, 255).astype(np.uint8)

# hist equalization
# img = run_histogram_equalization(img)

# resize bilinear
kfactor = 2
img = cv2.resize(img, (width // kfactor, height // kfactor), interpolation=cv2.INTER_LINEAR)

# save image
name = f"{input_name}_postprocess_.bmp"
Image.fromarray(img).save(name) # option 1 pillow
# if opencv convert rgb to bgr

######################################################
################# PLOT ##############################

# plot both images
plot_imgs(img, img_raw_RGB, flip)
plt.show()
# calculus of color contribution
red = int(img_demoisaic[:,:,0].sum())
green = int(img_demoisaic[:,:,1].sum())
blue = int(img_demoisaic[:,:,2].sum())

txt = f"blue: {blue}, green: {green}, red: {red}"
print(txt)