# Copyright 2023-2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import os
import cv2
import matplotlib.pyplot as plt
import numpy as np
from exifread.utils import Ratio
from skimage.metrics import peak_signal_noise_ratio
from skimage.metrics import structural_similarity as ssim
from pathlib import Path
import math

def gammaCorrection(src, gamma):
    invGamma = 1 / gamma

    table = [((i / 255) ** invGamma) * 255 for i in range(256)]
    table = np.array(table, np.uint8)
    return cv2.LUT(src, table)

def new_gamma_correction(img):
    mean = img.mean()
    gamma = math.log(0.5*255)/math.log(255*mean)
    print("gamma = ", gamma)

    # do gamma correction
    img_gamma1 = np.power(img, 1/gamma) #.clip(0,255).astype(np.uint8)
    return img_gamma1

def log_tranform(img):
    img = (255*img).astype(np.uint8)
    c = 160/np.log(1+img.max())
    img = c*np.log(1+img)
    img = img/255
    return img

def ch_op(ch):
    a = 0
    b = 1
    c = np.percentile(ch,2)
    d = np.percentile(ch,98)
    ratio = 0.5*((b-a)/(d-c))
    ch = (ch - c)*ratio
    return ch
    
def img_contrast(img):
    r = ch_op(img[:,:,0])
    g = ch_op(img[:,:,1])
    b = ch_op(img[:,:,2])
    img[:,:,0] = r
    img[:,:,1] = g
    img[:,:,2] = b
    return img
    
    
def pixel (img):
    img = img.astype(np.float64) 
    pixel = lambda x,y : {
        0: [ img[x][y] , (img[x][y-1] + img[x-1][y] + img[x+1][y] + img[x][y+1]) / 4 ,  (img[x-1][y-1] + img[x+1][y-1] + img[x-1][y+1] + img[x+1][y+1]) / 4 ] ,
        1: [ (img[x-1][y] + img[x+1][y])  / 2,img[x][y] , (img[x][y-1] + img[x][y+1]) / 2 ],
        2: [(img[x][y-1] + img[x][y+1]) / 2 ,img[x][y], (img[x-1][y] + img[x+1][y]) / 2],
        3: [(img[x-1][y-1] + img[x+1][y-1] + img[x-1][y+1] + img[x+1][y+1]) / 4 , (img[x][y-1] + img[x-1][y] + img[x+1][y] + img[x][y+1]) / 4 ,img[x][y] ]
    } [  x % 2 + (y % 2)*2]
    res = np.zeros ( [    np.size(img,0) , np.size(img,1)  , 3] )
    for x in range (1,np.size(img,0)-2):
        for y in range (1,np.size(img,1)-2):
            p = pixel(x,y)
            p.reverse();
            res[x][y] = p
    res = res.astype(np.uint8)
    return res

def unpack_mipi_raw10_data(byte_buf):
    data = np.frombuffer(byte_buf, dtype=np.uint8)
    # 5 bytes contain 4 10-bit pixels (5x8 == 4x10)
    b1, b2, b3, b4, b5 = np.reshape(
        data, (data.shape[0]//5, 5)).astype(np.uint16).T
    o1 = (b1 << 2) + ((b5) & 0x3)
    o2 = (b2 << 2) + ((b5 >> 2) & 0x3)
    o3 = (b3 << 2) + ((b5 >> 4) & 0x3)
    o4 = (b4 << 2) + ((b5 >> 6) & 0x3)
    unpacked = np.reshape(np.concatenate(
        (o1[:, None], o2[:, None], o3[:, None], o4[:, None]), axis=1),  4*o1.shape[0])
    return unpacked

def unpack_mipi_raw10_buffer(buffer):
    # 5 bytes contain 4 10-bit pixels (5x8 == 4x10)
    b1, b2, b3, b4, b5 = np.reshape(buffer, (buffer.shape[0]//5, 5)).astype(np.uint16).T
    o1 = (b1 << 2) + ((b5) & 0x3)      # B
    o2 = (b2 << 2) + ((b5 >> 2) & 0x3) # G
    o3 = (b3 << 2) + ((b5 >> 4) & 0x3) # G
    o4 = (b4 << 2) + ((b5 >> 6) & 0x3) # R
    unpacked = np.reshape(np.concatenate(
        (o1[:, None], 
         o2[:, None], 
         o3[:, None], 
         o4[:, None]), axis=1),  4*o1.shape[0])
    return unpacked


def unpack_mipi_raw8_buffer(buffer):
    # 5 bytes contain 4 10-bit pixels (5x8 == 4x10)
    R, G, G2, B = np.reshape(buffer, (buffer.shape[0]//4, 4)).astype(np.uint8).T
    concatenated = np.concatenate((R[:, None], G[:, None], G2[:, None], B[:, None]), axis=1)
    unpacked = np.reshape(concatenated,  4*R.shape[0])
    return unpacked

def unpack_mipi_raw10_buffer_dummy(buffer):
    # 5 bytes contain 4 10-bit pixels (5x8 == 4x10)
    b1, b2, b3, b4, b5 = np.reshape(
        buffer, (buffer.shape[0]//5, 5)).astype(np.uint16).T
    o1 = b1 << 2  
    o2 = b2 << 2  
    o3 = b3 << 2  
    o4 = b4 << 2  
    unpacked = np.reshape(np.concatenate(
        (o1[:, None], o2[:, None], o3[:, None], o4[:, None]), axis=1),  4*o1.shape[0])
    return unpacked

def split_channels(buffer):
    # 5 bytes contain 4 10-bit pixels (5x8 == 4x10)
    b1, b2, b3, b4 = np.reshape(
        buffer, (buffer.shape[0]//4, 4)).astype(np.uint16).T
    o1 = b1 << 2  
    o2 = b2 << 2  
    o3 = b3 << 2  
    o4 = b4 << 2  
    unpacked = np.reshape(np.concatenate(
        (o1[:, None], o2[:, None], o3[:, None], o4[:, None]), axis=1),  4*o1.shape[0])
    return unpacked

def align_down(size, align):
    return (size & ~((align)-1)) # 831 , 32 --> 800

def align_up(size, align): # 800, 32 // it just searches the nearest 32 or 16 bits
    return align_down(size + align - 1, align) # 831 , 32

def remove_padding(data, width, height, bit_width):
    buff = np.frombuffer(data, np.uint8)
    real_width = int(width / 8 * bit_width)
    # align_width = align_up(real_width, 32)
    # align_height = align_up(height, 16)
    align_height = height
    align_width = width
    
    buff = buff.reshape(align_height, align_width)
    buff = buff[:height, :real_width] # croping
    buff = buff.reshape(height * real_width) # serialise
    return buff

def remove_padding_buffer(buff, width, height, bit_width):
    real_width = int(width / 8 * bit_width) # 800 in raw 10 
    align_width = align_up(real_width, 32)
    align_height = align_up(height, 16)
    
    buff = buff.reshape(align_height, align_width)
    buff = buff[:height, :real_width]
    print(real_width)
    buff = buff.reshape(height * real_width)
    return buff

def remove_padding_buffer_no_align(buff, width, height, bit_width):
    real_width = int(width / 8 * bit_width) # 800 in raw 10 
    #align_width = align_up(real_width, 32)
    #align_height = align_up(height, 16)
    
    buff = buff.reshape(height, real_width)
    buff = buff[:height, :real_width]
    print(real_width)
    buff = buff.reshape(height * real_width)
    return buff

def scale(img):
    img = ((img - img.min()) * (1/(img.max() - img.min()) * 255)).astype('uint8')
    return img
    
def ratios2floats(ratios):
    floats = []
    for ratio in ratios:
        floats.append(float(ratio.num) / ratio.den)
    return floats

def normalize(raw_image, black_level, white_level, dtype=np.uint16):
    black_level_mask = black_level
    normalized_image = raw_image.astype(dtype) - black_level_mask
    normalized_image[normalized_image < 0] = 0
    normalized_image = normalized_image / (white_level - black_level_mask)
    return normalized_image

def old_normalize(raw_image, black_level, white_level):
    if type(black_level) is list and len(black_level) == 1:
        black_level = float(black_level[0])
    if type(white_level) is list and len(white_level) == 1:
        white_level = float(white_level[0])
    black_level_mask = black_level
    if type(black_level) is list and len(black_level) == 4:
        if type(black_level[0]) is Ratio:
            black_level = ratios2floats(black_level)
        black_level_mask = np.zeros(raw_image.shape)
        idx2by2 = [[0, 0], [0, 1], [1, 0], [1, 1]]
        step2 = 2
        for i, idx in enumerate(idx2by2):
            black_level_mask[idx[0]::step2, idx[1]::step2] = black_level[i]
    normalized_image = raw_image.astype(np.float32) - black_level_mask
    # if some values were smaller than black level
    normalized_image[normalized_image < 0] = 0
    normalized_image = normalized_image / (white_level - black_level_mask)
    return normalized_image

def lens_shading_correction(raw_image, gain_map_opcode, bayer_pattern, gain_map=None, clip=True):
    """
    Apply lens shading correction map.
    :param raw_image: Input normalized (in [0, 1]) raw image.
    :param gain_map_opcode: Gain map opcode.
    :param bayer_pattern: Bayer pattern (RGGB, GRBG, ...).
    :param gain_map: Optional gain map to replace gain_map_opcode. 1 or 4 channels in order: R, Gr, Gb, and B.
    :param clip: Whether to clip result image to [0, 1].
    :return: Image with gain map applied; lens shading corrected.
    """

    if gain_map is None and gain_map_opcode:
        gain_map = gain_map_opcode.data['map_gain_2d']

    # resize gain map, make it 4 channels, if needed
    gain_map = cv2.resize(gain_map, dsize=(raw_image.shape[1] // 2, raw_image.shape[0] // 2),
                          interpolation=cv2.INTER_LINEAR)
    if len(gain_map.shape) == 2:
        gain_map = np.tile(gain_map[..., np.newaxis], [1, 1, 4])

    if gain_map_opcode:
        # TODO: consider other parameters

        top = gain_map_opcode.data['top']
        left = gain_map_opcode.data['left']
        bottom = gain_map_opcode.data['bottom']
        right = gain_map_opcode.data['right']
        rp = gain_map_opcode.data['row_pitch']
        cp = gain_map_opcode.data['col_pitch']

        gm_w = right - left
        gm_h = bottom - top

        # gain_map = cv2.resize(gain_map, dsize=(gm_w, gm_h), interpolation=cv2.INTER_LINEAR)

        # TODO
        # if top > 0:
        #     pass
        # elif left > 0:
        #     left_col = gain_map[:, 0:1]
        #     rep_left_col = np.tile(left_col, [1, left])
        #     gain_map = np.concatenate([rep_left_col, gain_map], axis=1)
        # elif bottom < raw_image.shape[0]:
        #     pass
        # elif right < raw_image.shape[1]:
        #     pass

    result_image = raw_image.copy()

    # one channel
    # result_image[::rp, ::cp] *= gain_map[::rp, ::cp]

    # per bayer channel
    upper_left_idx = [[0, 0], [0, 1], [1, 0], [1, 1]]
    bayer_pattern_idx = np.array(bayer_pattern)
    # blue channel index --> 3
    bayer_pattern_idx[bayer_pattern_idx == 2] = 3
    # second green channel index --> 2
    if bayer_pattern_idx[3] == 1:
        bayer_pattern_idx[3] = 2
    else:
        bayer_pattern_idx[2] = 2
    for c in range(4):
        i0 = upper_left_idx[c][0]
        j0 = upper_left_idx[c][1]
        result_image[i0::2, j0::2] *= gain_map[:, :, bayer_pattern_idx[c]]

    if clip:
        result_image = np.clip(result_image, 0.0, 1.0)

    return result_image

def white_balance(normalized_image, as_shot_neutral, cfa_pattern):
    #if type(as_shot_neutral[0]) is Ratio:
    #    as_shot_neutral = ratios2floats(as_shot_neutral)
    idx2by2 = [[0, 0], [0, 1], [1, 0], [1, 1]]
    step2 = 2
    white_balanced_image = np.zeros(normalized_image.shape)
    for i, idx in enumerate(idx2by2):
        idx_y = idx[0]
        idx_x = idx[1]
        white_balanced_image[idx_y::step2, idx_x::step2] = normalized_image[idx_y::step2, idx_x::step2] / as_shot_neutral[cfa_pattern[i]]
    white_balanced_image = np.clip(white_balanced_image, 0.0, 1.0)
    return white_balanced_image

def simple_white_balance(norm_img, as_shot_neutral=None, cfa_pattern=[2, 1, 1, 0]): #RGGB
    if as_shot_neutral is None:
        as_shot_neutral = [0.5666090846, 1, 0.7082979679] 

    white_balanced_image = np.zeros(norm_img.shape)
    pairs = [[0, 0], [0, 1], [1, 0], [1, 1]]
    for i,pair in enumerate(pairs):
        idx_y, idx_x = pair
        white_balanced_image[idx_y::2, idx_x::2] = norm_img[idx_y::2, idx_x::2] / as_shot_neutral[cfa_pattern[i]] 
    
    white_balanced_image = np.clip(white_balanced_image, 0.0, 1.0)
    return white_balanced_image

def get_opencv_demsaic_flag(cfa_pattern, output_channel_order, alg_type='VNG'):
    # using opencv edge-aware demosaicing
    #  !!!!! CAREFUL OPENCV REVERSE BYTE ORDERS WHEN DEMOSAICING !!!!!
    if alg_type != '':
        alg_type = '_' + alg_type
    if output_channel_order == 'BGR':
        if cfa_pattern == [0, 1, 1, 2]:  # RGGB  # 
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_BG2BGR' + alg_type)
        elif cfa_pattern == [2, 1, 1, 0]:  # BGGR
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_RG2BGR' + alg_type)
        elif cfa_pattern == [1, 0, 2, 1]:  # GRBG
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_GB2BGR' + alg_type)
        elif cfa_pattern == [1, 2, 0, 1]:  # GBRG
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_GR2BGR' + alg_type)
        else:
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_BG2BGR' + alg_type)
            print("CFA pattern not identified.")
    else:  # RGB
        if cfa_pattern == [0, 1, 1, 2]:  # RGGB
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_BG2RGB' + alg_type) ## THIS ONE <<<<<<<<<
        elif cfa_pattern == [2, 1, 1, 0]:  # BGGR
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_RG2RGB' + alg_type)
        elif cfa_pattern == [1, 0, 2, 1]:  # GRBG
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_GB2RGB' + alg_type)
        elif cfa_pattern == [1, 2, 0, 1]:  # GBRG
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_GR2RGB' + alg_type)
        else:
            opencv_demosaic_flag = eval('cv2.COLOR_BAYER_BG2RGB' + alg_type)
            print("CFA pattern not identified.")
    return opencv_demosaic_flag


def demosaic(white_balanced_image, cfa_pattern, output_channel_order='BGR', alg_type='VNG', clip_float=True):
    """
    https://docs.opencv.org/3.4/de/d25/imgproc_color_conversions.html
    Demosaic a Bayer image.
    :param white_balanced_image:
    :param cfa_pattern:
    :param output_channel_order:
    :param alg_type: algorithm type. options: '', 'EA' for edge-aware, 'VNG' for variable number of gradients
    :return: Demosaiced image
    """
    if alg_type == 'VNG':
        max_val = 255
        wb_image = (white_balanced_image * max_val).astype(dtype=np.uint8)
    else:
        max_val = 16383
        wb_image = (white_balanced_image * max_val).astype(dtype=np.uint16)

    if alg_type in ['', 'EA', 'VNG']:
        opencv_demosaic_flag = get_opencv_demsaic_flag(cfa_pattern, output_channel_order, alg_type=alg_type)
        demosaiced_image = cv2.cvtColor(wb_image, opencv_demosaic_flag)
        
    if clip_float:
        demosaiced_image = demosaiced_image.astype(dtype=np.float32) / max_val
    return demosaiced_image





def old_apply_color_space_transform(demosaiced_image, color_matrix_1=None, clip_float=True):
    if color_matrix_1 is None: # color space transformation to XYZ
        color_matrix_1 = [0.9762914777, -0.2504389584, -0.1018426344, 
                        -0.1751390547, 0.9807397723, 0.1705771685, 
                        0.04482413828, 0.1344814152, 0.4878755212]
    
    xyz2cam1 = np.reshape(np.asarray(color_matrix_1), (3, 3))
    # normalize rows (needed?)
    xyz2cam1 = xyz2cam1 / np.sum(xyz2cam1, axis=1, keepdims=True)
    # inverse
    cam2xyz1 = np.linalg.inv(xyz2cam1)
    # simplified matrix multiplication
    xyz_image = cam2xyz1[np.newaxis, np.newaxis, :, :] * demosaiced_image[:, :, np.newaxis, :]
    xyz_image = np.sum(xyz_image, axis=-1)
    if clip_float:
        xyz_image = np.clip(xyz_image, 0.0, 1.0)
    else:
        xyz_image = np.clip(xyz_image, 0, 255)
    return xyz_image


def apply_color_space_transform(demosaiced_image, color_matrix_1=None, clip_float=True): #TO CIE XYZ
    if color_matrix_1 is None: # color space transformation to XYZ
        color_matrix_1 = np.array(
        [   [ 0.66369444,  0.24726221,  0.08904335],
            [ 0.13562966,  1.09600039, -0.23163006],
            [-0.09836362, -0.32482671,  1.42319032]])
        
    xyz_image = np.tensordot(demosaiced_image, color_matrix_1, axes=(-1, -1))
    
    if clip_float:
        xyz_image = np.clip(xyz_image, 0.0, 1.0)
    else:
        xyz_image = np.clip(xyz_image, 0, 255)
    return xyz_image


def transform_xyz_to_srgb(xyz_image, clip_float=True):
    color_matrix_2 = np.array(
       [[ 2.68965507, -1.27586199, -0.41379307],
       [-1.02210817,  1.97828664,  0.04382154],
       [ 0.06122446, -0.22448978,  1.16326533]])
    
    srgb_image = np.tensordot(xyz_image, color_matrix_2, axes=(-1, -1))
    
    if clip_float:
        srgb_image = np.clip(srgb_image, 0.0, 1.0)
    else:
        srgb_image = np.clip(srgb_image, 0, 255)
    return srgb_image

def old_transform_xyz_to_srgb(xyz_image, clip_float=True):
    color_matrix_2 = np.array([[3.2404542, -1.5371385, -0.4985314],
                                [-0.9692660, 1.8760108, 0.0415560],
                                [0.0556434, -0.2040259, 1.0572252]])
    # normalize rows
    color_matrix_2 = color_matrix_2 / np.sum(color_matrix_2, axis=-1, keepdims=True)

    srgb_image = color_matrix_2[np.newaxis, np.newaxis, :, :] * xyz_image[:, :, np.newaxis, :]
    srgb_image = np.sum(srgb_image, axis=-1)
    
    
    if clip_float:
        srgb_image = np.clip(srgb_image, 0.0, 1.0)
    else:
        srgb_image = np.clip(srgb_image, 0, 255)
    return srgb_image

def apply_tone_map(x):
    # simple tone curve
    # return 3 * x ** 2 - 2 * x ** 3

    # tone_curve = loadmat('tone_curve.mat')
    tone_curve = loadmat(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'tone_curve.mat'))
    tone_curve = tone_curve['tc']
    x = np.round(x * (len(tone_curve) - 1)).astype(int)
    tone_mapped_image = np.squeeze(tone_curve[x])
    return tone_mapped_image


def old_run_histogram_equalization(img_bgr):
    img_yuv = cv2.cvtColor(img_bgr, cv2.COLOR_BGR2YUV)
    # equalize the histogram of the Y channel
    img_yuv[:,:,0] = cv2.equalizeHist(img_yuv[:,:,0])
    # convert the YUV image back to RGB format
    img_output = cv2.cvtColor(img_yuv, cv2.COLOR_YUV2BGR)
    return img_output 


def run_histogram_equalization(img_bgr):
    clahe_model = cv2.createCLAHE(clipLimit=2, tileGridSize=(3,3))
    # For ease of understanding, we explicitly equalize each channel individually
    colorimage_b = clahe_model.apply(img_bgr[:,:,0])
    colorimage_g = clahe_model.apply(img_bgr[:,:,1])
    colorimage_r = clahe_model.apply(img_bgr[:,:,2])
    colorimage_clahe = np.stack((colorimage_b,colorimage_g,colorimage_r), axis=2)
    return colorimage_clahe


def show_histogram_by_channel(image, ylim=None):
    # Set the histogram bins to 256, the range to 0-255
    hist_size = 265
    hist_range = (0, 255)

    # Plot the histograms using plt.hist
    plt.figure(figsize=(10, 5))
    for i, col in enumerate(['r', 'g', 'b']):
        plt.subplot(1, 3, i+1)
        plt.title(f'{col.upper()} Histogram')
        plt.xlim([0, hist_size])
        if ylim is not None:
            plt.ylim([0, ylim])
        plt.hist(image[:,:,i].ravel(), bins=hist_size, range=hist_range, color=col)
    plt.show()


def show_histogram_by_channel_ax(image, ax):
    # Set the histogram bins to 256, the range to 0-255
    hist_size = 256
    hist_range = (0, hist_size)

    # Plot the histograms using plt.hist
    for i, col in enumerate(['r', 'g', 'b']):
        ax.axis(xmin=-1,xmax=256)
        ax.hist(image[:,:,i].ravel(), bins=hist_size, range=hist_range, color=col)


def plot_imgs(img, img_raw_RGB, flip, ax=None):
    hist_size = 256
    if ax is None:
        fig, ax = plt.subplots(2, 2, figsize=(16, 8))
    if flip:
        img  = cv2.flip(img, 0)
        img_raw_RGB  = cv2.flip(img_raw_RGB, 0)
        
    ((ax1, ax2), (ax3, ax4)) = ax
    ax1.imshow(img_raw_RGB)
    ax1.set_title('img_raw_RGB')
    ax2.imshow(img)
    ax2.set_title('img_post_processed')
    # plt.show()

    # plot histogram
    ax3.hist(img.mean(axis=2).flatten(), hist_size)
    ax3.hist(img_raw_RGB.mean(axis=2).flatten(), hist_size)
    ax3.axis(xmin=0,xmax=hist_size)
    ax3.legend(["processed", "unprocessed"])

    # show histogram for 3 channels
    show_histogram_by_channel_ax(img, ax4)
    
    
def split_into_bytes(buffer):
    byte_array = np.zeros(800*480, dtype=np.uint8)
    for i in range(len(buffer)):
        byte_array[i*4] = buffer[i] >> 24
        byte_array[i*4 + 1] = (buffer[i] >> 16) & 0xFF
        byte_array[i*4 + 2] = (buffer[i] >> 8) & 0xFF
        byte_array[i*4 + 3] = buffer[i] & 0xFF
    return byte_array


def downscale_planes(buffer, height, width, alg=None):
    from skimage.transform import downscale_local_mean, resize
    
    if alg is None:
        return downscale_resize(buffer, height, width)
    elif alg == 'gaussian':
        return downscale_gaussian(buffer, height, width)
    else:
        ns = (2, 2)
        new_shape = np.array((height, width)) // np.array(ns)    
        
        R, G, G2, B =  np.reshape(buffer, (buffer.shape[0]//4, 4)).astype(np.uint8).T
        R  = np.array(downscale_local_mean(R.reshape((height, width//4)), ns)).astype(np.uint8).reshape(-1,)
        G  = np.array(downscale_local_mean(G.reshape((height, width//4)), ns)).astype(np.uint8).reshape(-1,)
        G2 = np.array(downscale_local_mean(G2.reshape((height, width//4)), ns)).astype(np.uint8).reshape(-1,)
        B  = np.array(downscale_local_mean(B.reshape((height, width//4)), ns)).astype(np.uint8).reshape(-1,)

        # output the data
        concatenated = np.concatenate((R[:, None], G[:, None], G2[:, None], B[:, None]), axis=1)
        shaped = np.reshape(concatenated,  4*R.shape[0]).astype(np.uint8)
        return shaped, new_shape



def downscale_resize(buffer, height, width):
    from skimage.transform import downscale_local_mean, resize
    R, G, G2, B =  np.reshape(buffer, (buffer.shape[0]//4, 4)).astype(np.uint8).T
    
    # reshape each channel 
    newsize = (height, width//4) # because 4 colors
    R  = np.reshape(R, newsize)
    G  = np.reshape(G, newsize)
    G2 = np.reshape(G2, newsize)
    B  = np.reshape(B, newsize)
    
    # downscale the image by half
    downscale_shape = (R.shape[0]//2, R.shape[1]//2)
    
    R  = resize(R, downscale_shape, preserve_range=True, anti_aliasing=True).astype(np.uint8).reshape(-1,)
    G  = resize(G, downscale_shape, preserve_range=True, anti_aliasing=True).astype(np.uint8).reshape(-1,)
    G2 = resize(G2, downscale_shape, preserve_range=True, anti_aliasing=True).astype(np.uint8).reshape(-1,)
    B  = resize(B, downscale_shape, preserve_range=True, anti_aliasing=True).astype(np.uint8).reshape(-1,)
    
    # output the data
    concatenated = np.concatenate((R[:, None], G[:, None], G2[:, None], B[:, None]), axis=1)
    shaped = np.reshape(concatenated,  4*R.shape[0]).astype(np.uint8)
    final_shape = shaped.reshape(height//2, width//2).shape
    return shaped, final_shape


def gkern(l=5, sig=1.):
    """\
    creates gaussian kernel with side length `l` and a sigma of `sig`
    """
    ax = np.linspace(-(l - 1) / 2., (l - 1) / 2., l)
    gauss = np.exp(-0.5 * np.square(ax) / np.square(sig))
    kernel = np.outer(gauss, gauss)
    return kernel / np.sum(kernel)
    
def downscale_gaussian(buffer, height, width):
    print("im doing gaussian reduction")
    from skimage.transform import downscale_local_mean, resize
    R, G, G2, B =  np.reshape(buffer, (buffer.shape[0]//4, 4)).astype(np.uint8).T
    
    # reshape each channel 
    newsize = (height, width//4) # because 4 colors
    R  = np.reshape(R, newsize)
    G  = np.reshape(G, newsize)
    G2 = np.reshape(G2, newsize)
    B  = np.reshape(B, newsize)
    
    
    # Define the Gaussian filter kernel
    kernel = gkern(3, 0.01)
    
    # Apply the Gaussian filter to the image
    Bp = B
    Rp = R
    R  = cv2.filter2D(R, -1, kernel)[::2, ::2].astype(np.uint8).reshape(-1,)
    G  = cv2.filter2D(G, -1, kernel)[::2, ::2].astype(np.uint8).reshape(-1,)
    G2 = cv2.filter2D(G2, -1, kernel)[::2, ::2].astype(np.uint8).reshape(-1,)
    B  = cv2.filter2D(B, -1, kernel)[::2, ::2].astype(np.uint8).reshape(-1,)
    
    # output the data
    concatenated = np.concatenate((R[:, None], G[:, None], G2[:, None], B[:, None]), axis=1)
    shaped = np.reshape(concatenated,  4*R.shape[0]).astype(np.uint8)
    final_shape = shaped.reshape(height//2, width//2).shape
    return shaped, final_shape


    
def downscale_image_vertically(image, k=4):
    # Define the kernel for the Gaussian filter
    ksize, sigma = 3,0.2
    kernel = cv2.getGaussianKernel(ksize, sigma)
    # kernel = np.array([1,2,1]).T
    kernel = (kernel / np.sum(kernel))
    # Apply the filter using filter2D
    image_downscaled = cv2.filter2D(image, -1, kernel)
    image_downscaled = image_downscaled[::k, :] # Update the image height to the new height
    return image_downscaled

def downscale_image_horizontally(image, k=2):
    # Define the kernel for the Gaussian filter
    ksize, sigma = 3,0.2
    kernel = cv2.getGaussianKernel(ksize, sigma)
    # kernel = np.array([1,2,1]).T
    kernel = (kernel / np.sum(kernel)).T
    # Apply the filter using filter2D
    image_downscaled = cv2.filter2D(image, -1, kernel)
    image_downscaled = image_downscaled[::, ::k] # Update the image height to the new height
    return image_downscaled

def downscale_image_both(image, k=2):
    kernel = gkern(7, 1)
    image_filtered = cv2.filter2D(image, -1, kernel)
    image_downscaled = image_filtered[::k, ::k]
    return image_downscaled

def downscale_image_full(image, kw=2, kh=2):
    kernel = gkern(5, 1)
    image_filtered = cv2.filter2D(image, -1, kernel)
    image_downscaled = image_filtered[::k, ::k]
    return image_downscaled

def downscale_resize_interp(image, k=2):
    img_height, img_width = image.shape[:2]
    height, width = img_height//k, img_width//k
    image = cv2.resize(image, (width, height), interpolation=cv2.INTER_LINEAR).astype(np.uint8)
    return image

def get_test_image():
    R = 100
    G = 120
    B = 140
    RGGB = np.array([R, G, G, B])

    height = 8
    width  = 16

    final_image = np.tile(RGGB, width*height//len(RGGB))
    print(final_image)
    return final_image, (height, width)


def get_test_image2():
    height = 16
    width  = 32
    final_image = np.array(range(height*width))
    return final_image, (height, width)

def get_real_image(path=None):
    if path is None:
        top_path   = Path(__file__).resolve().parent
        imgs_path  = os.path.join(top_path, "test_imgs/") #TODO .env
        input_name = imgs_path + "img_raw8_640_480_cube3.xbin"
        
    width = 640
    height = 480

    with open(input_name, "rb") as f:
        data = f.read()
        buffer = np.frombuffer(data, dtype=np.uint8)
        
    return buffer, (height, width)


def get_image_path(path):
    width = 640
    height = 480

    with open(path, "rb") as f:
        data = f.read()
        buffer = np.frombuffer(data, dtype=np.uint8)
        
    return buffer, (height, width)


def downsample_channels(channels, k=4):
    def channel_op(channel):
        # channel = downscale_image_vertically(channel, k)
        # channel = downscale_image_both(channel, k) 
        # channel = bilinear_resize(channel, k)
        channel = downscale_resize_interp(channel, k)
        return channel
    it = map(channel_op, channels)
    dchannels = np.array(list(it))
    height, width = dchannels[0].shape
    return dchannels, (height*2, width*2) #because we are going to fill the double of image dimensions


import math


def bilinear_resize(image, k):
  """
  `image` is a 2-D numpy array
  `height` and `width` are the desired spatial dimension of the new 2-D array.
  """
  img_height, img_width = image.shape[:2]
  height, width = img_height//k, img_width//k
  resized = np.empty([height, width])

  x_ratio = float(img_width - 1) / (width - 1) if width > 1 else 0
  y_ratio = float(img_height - 1) / (height - 1) if height > 1 else 0

  for i in range(height):
    for j in range(width):

      x_l, y_l = math.floor(x_ratio * j), math.floor(y_ratio * i)
      x_h, y_h = math.ceil(x_ratio * j), math.ceil(y_ratio * i)

      x_weight = (x_ratio * j) - x_l
      y_weight = (y_ratio * i) - y_l

      a = image[y_l, x_l]
      b = image[y_l, x_h]
      c = image[y_h, x_l]
      d = image[y_h, x_h]

      pixel = a * (1 - x_weight) * (1 - y_weight) \
          + b * x_weight * (1 - y_weight) + \
          c * y_weight * (1 - x_weight) + \
          d * x_weight * y_weight

      resized[i][j] = pixel

  return resized



def get_color_rgb(col, row):
    RED = 0
    GREEN = 1
    BLUE = 2
    color_table = [
        [RED, GREEN],
        [GREEN, BLUE]
    ]
    return color_table[col & 1][row & 1]


def split_planes(img):
    r   = img[0::2, 0::2]
    g1  = img[0::2, 1::2]
    g2  = img[1::2, 0::2]
    b   = img[1::2, 1::2]
    return np.array((r,g1,g2,b))
            
            
def reverse_split_planes(channels, height, width):
    img = np.zeros((height, width))
    img[0::2, 0::2] = channels[0] #R
    img[0::2, 1::2] = channels[1] #G
    img[1::2, 0::2] = channels[2] #G
    img[1::2, 1::2] = channels[3] #B
    return img
            
            
            
def pipeline(img, demosaic_opt=True): #it takes a RAW IMAGE
    img = img.astype(np.uint8)
    as_shot_neutral = [0.566090846, 1, 0.7082979679]
    as_shot_neutral = [0.766090846, 1, 0.7082979679]
    # as_shot_neutral = [1, 1, 1]
    cfa_pattern = [0, 1, 1, 2] 
    # black level substraction
    img = normalize(img, 15, 254, np.uint8)  
    # demosaic
    if demosaic_opt:
        img  = demosaic(img, cfa_pattern, output_channel_order='RGB', alg_type='VNG')
    else:
        # demosaic avoiding blue
        channels = split_planes(img)
        h,w = channels.shape[1:]
        rgb = np.zeros((h,w,3))
        rgb[:,:,0] = channels[0,:,:]
        rgb[:,:,1] = channels[1,:,:]
        rgb[:,:,2] = channels[3,:,:]
        img = rgb
    
    # white balancing
    # img = simple_white_balance(img, as_shot_neutral, cfa_pattern)
    img = gray_world(img)
    # color transforms
    #img = apply_color_space_transform(img)
    #img = transform_xyz_to_srgb(img)
    # gamma
    img = img ** (1.0 / 2.2)
    # clip the image
    img = np.clip(255*img, 0, 255).astype(np.uint8)
    # hist equalization
    # img = run_histogram_equalization(img)
    return img

def pipeline_raw8(img, demosaic_opt=True): #it takes a RAW IMAGE
    as_shot_neutral = [0.6301882863, 1, 0.6555861831]
    width, height = 640, 480
    cfa_pattern = [0, 1, 1, 2] # explorer board

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
    return img


def pipeline_nodemosaic(img):
    img = img.astype(np.uint8)
    as_shot_neutral = [0.566090846, 1, 0.7082979679]
    as_shot_neutral = [0.766090846, 1, 0.7082979679]
    # as_shot_neutral = [1, 1, 1]
    cfa_pattern = [0, 1, 1, 2] 
    # black level substraction
    img = normalize(img, 15, 254, np.uint8)  
    # white balancing
    img = simple_white_balance(img, as_shot_neutral, cfa_pattern)
    # gamma
    img = img ** (1.0 / 2)
    # clip the image
    img = np.clip(255*img, 0, 255).astype(np.uint8)
    # hist equalization
    # img = run_histogram_equalization(img)
    return img


def mult_temp(M,img):
    R,G,B = img[:,:,0], img[:,:,1], img[:,:,2] 
    a1,a2,a3,a4,a5,a6,a7,a8,a9 = M.flatten()
    
    X = a1*R + a2*G + a3*B
    Y = a4*R + a5*G + a6*B
    Z = a7*R + a8*G + a9*B
    
    X = X[..., np.newaxis]
    Y = Y[..., np.newaxis]
    Z = Z[..., np.newaxis]
    
    f = np.concatenate((X,Y,Z), axis=-1).reshape(img.shape)
    f.clip(0,1)
    return f

def new_color_correction(img):
    RAW_to_XYZ = np.array(
                [[0.66369444,  0.24726221,  0.08904335],
                [ 0.13562966,  1.09600039, -0.23163006],
                [-0.09836362, -0.32482671,  1.42319032]]
                ).reshape(3,3)     
    RAW_to_XYZ = RAW_to_XYZ / np.sum(RAW_to_XYZ, axis=-1, keepdims=True)
    
    XYZ_to_sGRB = np.array(
                [[3.2404542, -1.5371385, -0.4985314],
                [-0.9692660,  1.8760108,  0.0415560],
                [0.0556434,  -0.2040259,  1.0572252]]
                ).reshape(3,3)
    XYZ_to_sGRB = XYZ_to_sGRB / np.sum(XYZ_to_sGRB, axis=-1, keepdims=True)
    
    # multiply
    img_xyz  = mult_temp(RAW_to_XYZ, img)
    img_srgb = mult_temp(XYZ_to_sGRB, img_xyz)    
    return img_srgb

def gray_world(img):
    Ravg = img[:,:,0].mean()
    Gavg = img[:,:,1].mean()
    Bavg = img[:,:,2].mean()

    alfa = Gavg/Ravg
    beta = Gavg/Bavg

    img[:,:,0] = alfa*img[:,:,0]
    img[:,:,2] = beta*img[:,:,2]
    # img[:,:,1] = 
    return img


def iterative_wb(img):
    img = img/255.0
    
    R =  img[:,:,0]
    G =  img[:,:,1]
    B =  img[:,:,2]
    
    # to YUV
    a,b,c = 0.299,0.587,0.114
    d,e,f = -0.147,-0.289,0.436
    g,h,i = 0.615,-0.515,-0.100
    
    y = a*R + b*G + c*B
    u = d*R + e*G + f*B
    v = g*R + h*G + i*B
    
    loc = np.where(y > 0.4) # find high luminance values
    
    # compute luminance region
    yl = y[loc]
    ul = u[loc]
    vl = v[loc]
    
    # local to RGB
    R = yl + 1.140*vl
    G = yl - 0.395*ul - 0.581*vl
    B = yl + 2.032*ul
    
    img[:,:,0] /= R.mean()
    img[:,:,1] /= G.mean()
    img[:,:,2] /= B.mean()
    
    img = img.clip(0,1)*255.0
    return img


def compute_score(img_ref, img):
    # if image is color, convert to gray
    if img_ref.ndim == 3:
        img_ref = cv2.cvtColor(img_ref, cv2.COLOR_RGB2GRAY)
    if img.ndim == 3:
        img = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
    
    score = ssim(img_ref, img)
    return score
    

if __name__ == '__main__':
    pass
