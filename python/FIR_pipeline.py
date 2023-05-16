import cv2
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image  # just to avoid color BGR issues when writting
from scipy import signal

from utils import *

OFF     = 0
STEP    = 4
STEP_V  = 2

RED     = 0
GREEN   = 1
BLUE    = 2

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
    for u in range(-KH, KH + 1, 1):
        if(i+u >= img.shape[1]):
            u = 0
            
        px = img[j, i + u]
        kx = filter[u + KH]
        sump+=px*kx
    return sump

def convolve_v(j:int, i:int, img, filter:list):
    sump = 0
    for u in range(-KV, KV + 1, 1):
        if(j+u >= img.shape[0]):
            u = 0
        
        px = img[j + u, i]
        kx = filter[u + KV]
        sump+=px*kx
    return sump



def create_filter():
    fil_h_5t  = [0.00539473, 0, 0.18786418, 0, 0.61348217, 0, 0.18786418, 0, 0.00539473]
    fil_h_3t  = [0.04622150, 0, 0.90755700, 0, 0.04622150]
    
    sigma =  0.3*(3/2 - 1) + 0.5
    SIGMA  = 1
    TAPS_V = 5
    TAPS_H = 3
    
    v_filter    =  create_fir_filter(TAPS_V, 3)
    h_filter    =  create_fir_filter(TAPS_H, 2)
    #h_filter    = np.array([-1, 2, -1]) 
    #v_filter    =  create_gaussian_filter(TAPS_V, sigma)
    # h_filter    =  create_gaussian_filter(TAPS_H, px)
    h_filter     =  intercalate_zeros(h_filter)
    
    # horizontal
    h1 = [0.20872991, 0, 0.58254019, 0, 0.20872991]  
    h2 = [0.04622150, 0, 0.90755700, 0, 0.04622150]
    h3 = [0.00539473, 0, 0.18786418, 0, 0.61348217, 0, 0.18786418, 0, 0.00539473]

    # vertical
    v1 = [0.08626086, 0.23894391, 0.34959045, 0.23894391, 0.08626086]
    v2 = [0.0248892,  0.2528858,  0.44445  , 0.2528858,  0.0248892]

    h_filter = h1
    v_filter = v2
    
    
    KV = (len(v_filter) -1) //2
    KH = (len(h_filter) -1) //2
    
    #v_filter          = [0.0462215, 0.907557, 0.0462215];
    #h_filter          = [0.0462215, OFF, 0.907557, OFF, 0.0462215];
    #print(v_filter)
    #print(h_filter)

    
    assert(TAPS_H % 2 != 0)
    return h_filter, v_filter, KV, KH, TAPS_V, TAPS_H


if __name__ == '__main__':

    ENABLE_SHOW = True
    
    # get test_image
    img, (height, width) = get_real_image()
    img = img.reshape(height, width)
    
    imgh = np.zeros((height//2, width//4, 3))
    imgv = np.zeros((height//4, width//4, 3))

    # create the filters
    h_filter, v_filter, KV, KH, TAPS_V, TAPS_H = create_filter()
    print(h_filter, v_filter)
    
    # horizontal filtering
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

    # vertical filter
    new_h, new_w, ch = imgh.shape
    
    # white balancing
    imgh = gray_world(imgh)
    red, green, blue = imgh[:,:,RED], imgh[:,:,GREEN], imgh[:,:,BLUE]
    
    for i in range(new_w):
        for j in range(0, new_h , STEP_V):
            red_p   = convolve_v(j, i, red, v_filter)
            green_p = convolve_v(j, i, green, v_filter)
            blue_p  = convolve_v(j, i, blue, v_filter)
            imgv[j//STEP_V,i, RED]      = red_p
            imgv[j//STEP_V,i, GREEN]    = green_p
            imgv[j//STEP_V,i, BLUE]     = blue_p


    imgv = np.clip(imgv, 0, 255).astype(np.uint8)
    img = imgv
    
    
    # post processing
    #########################
    # black level substraction
    img = normalize(img, 15, 254, np.uint8)  
    
    # gamma
    img = img ** (1.0 / 2.2)
    #img = log_tranform(img)
    #img = new_gamma_correction(img)
    #img = img_contrast(img).clip(0,1)
    # sharpen
    kernel = np.array([[0, -1, 0],
                [-1, 10,-1],
                [0, -1, 0]])
    kernel = kernel / 4
    #img = cv2.filter2D(src=img, ddepth=-1, kernel=kernel)
    #img = new_color_correction(img)
    
    # clip the image
    img = np.clip(255*img, 0, 255).astype(np.uint8)
    ###################################
    
    plt.imshow(img)
    if ENABLE_SHOW:
        plt.show()

    # save image
    name = f"out_h{TAPS_H}_v{TAPS_V}.png"
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
        

