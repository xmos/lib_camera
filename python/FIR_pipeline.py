# Copyright 2023-2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import cv2
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image  # just to avoid color BGR issues when writting
from scipy import signal

from utils import (
    peak_signal_noise_ratio, 
    pipeline, 
    normalize, 
    gray_world, 
    get_real_image,
    new_color_correction,
    run_histogram_equalization,
    iterative_wb)

OFF     = 0
STEP    = 4
STEP_V  = 2

RED     = 0
GREEN   = 1
BLUE    = 2

kernel_sharpen = np.array([ 
                    [-1, -1, -1],
                    [-1, 10, -1],
                    [-1, -1, -1]])/10 # /10 to soft the response

kernel_sharpen_5 = np.array([-0.00391,-0.01563,-0.02344,-0.01563,-0.00391,-0.01563,-0.06250,-0.09375,-0.06250,-0.01563,-0.02344,-0.09375, 1.85980,-0.09375,-0.02344,-0.01563,-0.06250,-0.09375,-0.06250,-0.01563,-0.0391,-0.01563,-0.02344,-0.01563,-0.00391])
kernel_sharpen_5 = kernel_sharpen_5.reshape(5,5)

def calculate_2dft(input):
    # https://thepythoncodingbook.com/2021/08/30/2d-fourier-transform-in-python-and-fourier-synthesis-of-images/
    ft = np.fft.ifftshift(input)
    ft = np.fft.fft2(ft)
    return np.fft.fftshift(ft)

def create_gaussian_filter(taps=3, sigma=1):
    kernel = cv2.getGaussianKernel(taps, sigma)
    kernel /= sum(kernel)
    return kernel.T[0]

def create_fir_filter(taps=3, factor=2, aux=10):
    cutoff = 0.5 / factor  # 0.5 is the Nyquist frequency
    n = np.arange(taps)
    sinc_func = np.sinc(2 * cutoff * (n - (taps - 1) / 2))    
    window = np.hamming(taps)
    #window = np.kaiser(taps, aux)
    #window = np.blackman(taps)
    FIR = 0.5*sinc_func + 0.5*window
    #FIR = sinc_func*window
    #FIR = window
    
    FIR /= np.sum(FIR)
    #np.round(FIR, 8)
    return FIR

def intercalate_zeros(filter):
    pad_filter = np.zeros(2*len(filter) - 1)
    pad_filter[::2] = filter
    return pad_filter


def convolve_h(j:int, i:int, img, filter:list):
    sump = 0
    KH = (len(filter) -1) //2
    for u in range(-KH, KH + 1, 1):
        if(i+u >= img.shape[1]):
            u = 0
            
        px = img[j, i + u]
        kx = filter[u + KH]
        sump+=px*kx
    return sump

def convolve_v(j:int, i:int, img, filter:list):
    sump = 0
    KV = (len(filter) -1) //2
    for u in range(-KV, KV + 1, 1):
        if(j+u >= img.shape[0]):
            u = 0
        
        px = img[j + u, i]
        kx = filter[u + KV]
        sump+=px*kx
    return sump



def create_filter():
    # horizontal
    h1 = [0.20872991, 0, 0.58254019, 0, 0.20872991]  
    h2 = [0.04622150, 0, 0.90755700, 0, 0.04622150]
    h3 = [0.00539473, 0, 0.18786418, 0, 0.61348217, 0, 0.18786418, 0, 0.00539473]

    # vertical
    v1 = [0.08626086, 0.23894391, 0.34959045, 0.23894391, 0.08626086]
    v2 = [0.0248892,  0.2528858,  0.44445  , 0.2528858,  0.0248892]

    h_filter = h2
    v_filter = v2
    
    
    KV = (len(v_filter) -1) //2
    KH = (len(h_filter) -1) //2
        
    assert(KH % 2 == 0)
    return h_filter, v_filter, KV, KH


def horizontal_filer(img, h_filter, height, width):
    KH = (len(h_filter) -1) //2
    # horizontal filtering
    imgh = np.zeros((height//2, width//4, 3))
    for j in range(height):
        for i in range(0, width, STEP):
            pos = i//STEP
            if (j%2==0):
                red_p   = convolve_h(j, i, img, h_filter)
                green_p = convolve_h(j, i+1, img, h_filter)
                imgh[j//2, pos, RED]  = red_p
                imgh[j//2, pos, GREEN]  = green_p
            else:
                imgh[j//2, pos, BLUE]  = convolve_h(j, i+1, img, h_filter)
        
    imgh = np.clip(imgh, 0, 255).astype(np.uint8)
    return imgh


def vertical_filer(imgh,height, width, v_filter, red, green, blue):
# vertical filter
    new_h, new_w, ch = imgh.shape
    imgv = np.zeros((height//4, width//4, 3))
    for i in range(new_w):
        for j in range(0, new_h , STEP_V):
            red_p   = convolve_v(j, i, red, v_filter)
            green_p = convolve_v(j, i, green, v_filter)
            blue_p  = convolve_v(j, i, blue, v_filter)
            imgv[j//STEP_V,i, RED]      = red_p
            imgv[j//STEP_V,i, GREEN]    = green_p
            imgv[j//STEP_V,i, BLUE]     = blue_p


    imgv = np.clip(imgv, 0, 255).astype(np.uint8)
    return imgv


def FIR_pipeline_func(img, height, width, show=False):
    # create the filters
    h_filter, v_filter, KV, KH = create_filter()
    
    # horizontal filter
    imgh = horizontal_filer(img, h_filter, height, width)
    
    # white balancing
    #imgh = gray_world(imgh)
    imgh = iterative_wb(imgh)
    
    red, green, blue = imgh[:,:,RED], imgh[:,:,GREEN], imgh[:,:,BLUE]
    
    # vertical filtering
    img = vertical_filer(imgh,height, width, v_filter, red, green, blue)
    
    ########### post processing ##############
    # black level substraction
    BLACK_LEVEL = 16
    img = normalize(img, BLACK_LEVEL, 254, np.uint8)  
    
    # gamma
    img = img ** (1.0 / 1.8)
    
    # sharpen (optional)
    #kernel_sharpen = kernel_sharpen/np.sum(kernel_sharpen)
    #img = cv2.filter2D(src=img, ddepth=-1, kernel=kernel_sharpen_5)
    
    # Color correction (optional)
    # img = new_color_correction(img)
    
    # clip the image
    img = np.clip(255*img, 0, 255).astype(np.uint8)
    
    # image stretch 
    # img = stretch_histogram(img)
    
    # histeq
    # img = run_histogram_equalization(img)
    
    return img
    
def stretch_histogram(image):
    stretched_image = np.zeros_like(image)

    for i in range(3):  # Iterate over color channels (R, G, B)
        channel = image[:, :, i]

        # Calculate the minimum and maximum pixel values
        min_val = np.min(channel)
        max_val = np.max(channel)

        # Apply contrast stretching to the channel
        stretched_channel = ((channel - min_val) * (255.0 / (max_val - min_val))).astype(np.uint8)

        # Assign the stretched channel to the output image
        stretched_image[:, :, i] = stretched_channel

    return stretched_image

if __name__ == '__main__':

    ENABLE_IMSHOW = True
    
    # get test_image
    img, (height, width) = get_real_image()
    img = img.reshape(height, width)
    
    # create the filters
    h_filter, v_filter, KV, KH = create_filter()
    print(h_filter, v_filter)
    
    # horizontal filter
    imgh = horizontal_filer(img, h_filter, height, width)
    
    # white balancing
    imgh = gray_world(imgh)
    red, green, blue = imgh[:,:,RED], imgh[:,:,GREEN], imgh[:,:,BLUE]
    
    # vertical filtering
    img = vertical_filer(imgh,height, width, v_filter, red, green, blue)
    
    ########### post processing ##############
    # black level substraction
    img = normalize(img, 15, 254, np.uint8)  
    
    # gamma
    img = img ** (1.0 / 1.8)
    
    # sharpen (optional)
    kernel_sharpen = kernel_sharpen/np.sum(kernel_sharpen)
    img = cv2.filter2D(src=img, ddepth=-1, kernel=kernel_sharpen_5)
    
    # Color correction (optional)
    #img = new_color_correction(img)
    
    # clip the image
    img = np.clip(255*img, 0, 255).astype(np.uint8)
    

    ########### post processing ##############
    
    
    if ENABLE_IMSHOW:
        plt.imshow(img)
        plt.show()

    # save image
    name = f"out.png"
    #print(name)
    Image.fromarray(img).save(name) # option 1 pillow

    
    # psnr
    img2, (height, width) = get_real_image()
    img2 = img2.reshape(height, width)
    ref_image = pipeline(img2, False)
    kfactor = 4
    ref_image = cv2.resize(ref_image, (width // kfactor, height // kfactor), interpolation=cv2.INTER_AREA)


    img_psnr = peak_signal_noise_ratio(ref_image, img) 
    print(img_psnr)
