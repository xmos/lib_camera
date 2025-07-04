# Copyright 2024-2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import cv2
import shutil
import subprocess
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from PIL import Image  # To avoid color BGR issues when writing
from skimage.metrics import peak_signal_noise_ratio
from skimage.metrics import structural_similarity

from typing import Optional, Literal
from pydantic import BaseModel

from kernels import kernel_array_rgb2, kernel_array_rgb1, kernel_array_rgb4

cwd = Path(__file__).parent.absolute()
path_imgs = cwd / "imgs"

folder_in = path_imgs
folder_out = path_imgs
assert folder_in.exists(), f"Folder {folder_in} does not exist"

# this is the equivalent of the sensor size
# is the max raw size, can be changed later
SENSOR_HEIGHT = 200
SENSOR_WIDTH = 200


class ImgSize(BaseModel):
    height: int = 200
    width: int = 200
    channels: Literal[1, 2, 3]
    dtype: Literal[np.int8, np.uint8] = np.int8


# TODO this classes will eventually be part of the library at some point
# and will replace all small and independent python functions in "/python" folder
class ImageDecoder(object):
    def __init__(self, input_size: ImgSize):
        self.height = input_size.height
        self.width = input_size.width
        self.channels = input_size.channels
        self.dtype = input_size.dtype
        self.last_img = None
        self.in_mode = "raw8" if self.channels == 1 else "rgb"
        self.sns_height = SENSOR_HEIGHT
        self.sns_width = SENSOR_WIDTH

    def _imgread(self, input_name):
        with open(input_name, "rb") as f:
            data = f.read()
        buffer = np.frombuffer(data, dtype=self.dtype)

        if self.dtype == np.int8:
            buffer = buffer.astype(np.int16) + 128

        if self.channels == 1:  # raw8
            buffer = buffer.reshape(self.sns_height, self.sns_width, 1)
            if self.sns_height != self.height or self.sns_width != self.width:
                print("Cropping image")
                buffer = buffer[: self.height, : self.width]

        img = buffer.reshape(self.height, self.width, self.channels).astype(np.uint8)
        return img

    def _imgsave(self, img: np.ndarray, input_name, output_name):
        assert isinstance(img, np.ndarray), "Image must be a numpy array"
        img = np.clip(img, 0, 255).astype(np.uint8)
        img_pil = Image.fromarray(img)
        if output_name is None:
            output_name = Path(input_name).with_suffix(".png")
        img_pil.save(output_name)
        print("Image saved in:", output_name)
        self.last_img = img_pil
        return img_pil

    # ------------------ RAW8 ------------------
    def raw8_resize(self, in_path: Path, out_path: Path, out_size: ImgSize):
        with open(in_path, "rb") as f:
            data = f.read()

        buffer = np.frombuffer(data, dtype=self.dtype)
        buffer = buffer.reshape(self.sns_height, self.sns_width, 1)
        buffer = buffer[: out_size.height, : out_size.width, :]
        buffer = buffer.flatten()
        assert len(buffer) > 0, "Buffer is empty after resizing"
        with open(out_path, "wb") as f:
            f.write(buffer)
        print("Image saved in:", out_path)
        return buffer

    def raw8_to_rgbx(self, input_name=None, output_name=None, k_factor=2):
        dem_filter = Image.Resampling.LANCZOS
        img = self._imgread(input_name)
        img = cv2.cvtColor(img, cv2.COLOR_BayerBG2RGB)
        img_pil = Image.fromarray(img)
        if k_factor > 1:
            img_pil = img_pil.resize(
                (self.width // k_factor, self.height // k_factor), dem_filter
            )
        if output_name is None:
            output_name = Path(input_name).with_suffix(".png")
        img_pil.save(output_name)
        print("Image saved in:", output_name)
        self.last_img = img_pil
        return img_pil
    
    def raw8_to_yuv422(self, input_name, output_name):
        img = self._imgread(input_name)
        dest_shape = (self.width // 2, self.height // 2)
        img = cv2.cvtColor(img, cv2.COLOR_BayerBG2RGB) # to rgb
        img = cv2.resize(img, dest_shape, interpolation=cv2.INTER_LINEAR)
        # Do AWB uint8
        img = self.rgb_apply_static_wb_uint8(img)
        yuv = cv2.cvtColor(img, cv2.COLOR_RGB2YUV) # default opencv is bgr
        # Extract Y, U, V channels
        Y = yuv[:, :, 0]; U = yuv[:, :, 1]; V = yuv[:, :, 2]
        # Subsample U and V channels
        U_sub = U[:, ::2]  # Take every second U value
        V_sub = V[:, ::2]  # Take every second V value 
        # Interleave Y, U, and V in YUV422 format (Y1 U Y2 V)
        h, w = Y.shape
        yuv422 = np.zeros((h, 2*w), dtype=np.uint8)    
        yuv422[:, 0::4] = Y[:, ::2]   # Y1
        yuv422[:, 1::4] = U_sub       # U (shared)
        yuv422[:, 2::4] = Y[:, 1::2]  # Y2
        yuv422[:, 3::4] = V_sub       # V (shared)   
        yuv422.tofile(output_name)  # Saves raw YUV422 data
        print("Image saved in:", output_name)
        return yuv422
        
    def raw8_to_rgb1(self, input_name=None, output_name=None):
        return self.raw8_to_rgbx(input_name, output_name, 1)

    def raw8_to_rgb2(self, input_name=None, output_name=None):
        return self.raw8_to_rgbx(input_name, output_name, 2)

    def raw8_to_rgb4(self, input_name=None, output_name=None):
        return self.raw8_to_rgbx(input_name, output_name, 4)

    def check_kernels(self, kernel_arr):
        for kernel in kernel_arr:
            assert kernel.shape == (32,), "Kernel shape is not 32"
            assert kernel.sum() == 1.0, "Kernel does not sum 1.0"
        print("All kernels are valid")

    def raw8_to_rgb1_xcore(self, input_name: Path = None, output_name: Path = None):
        """This function mimics an xcore approach to convert from a raw8 image to rgb1.
        In RGB1 conversion each raw8 pixel is converted to 1RGB pixels.

        Args:
            input_name (Path, optional): input path to the raw image. Defaults to None.
            output_name (Path, optional): output path to the output rgb image. Defaults to None.

        Returns:
            Image: rgb image in Pillow format.
        """
        kernels = kernel_array_rgb1
        kernels = kernels / 4.0
        self.check_kernels(kernels)
        img = self._imgread(input_name)
        out_size = (self.height, self.width, 3)
        img_out = np.zeros(out_size, dtype=np.float32)
        for j in range(0, self.height - 2, 2):
            for i in range(0, self.width, 8):
                block_4x8 = img[j : j + 4, i : i + 8].flatten()
                block_output = np.zeros((48), dtype=np.float32)
                for x, kernel in enumerate(kernels):
                    block_output[x] = np.dot(block_4x8, kernel.flatten())
                block_output = block_output.reshape((2, 8, 3))
                img_out[j + 1 : j + 3, i : i + 8, 0] = block_output[:, :, 0]
                img_out[j + 1 : j + 3, i : i + 8, 1] = block_output[:, :, 1]
                img_out[j + 1 : j + 3, i : i + 8, 2] = block_output[:, :, 2]

        img_out = img_out.reshape(out_size)
        img_out_pil = self._imgsave(img_out, input_name, output_name)
        return img_out_pil

    def raw8_to_rgb2_xcore(self, input_name: Path = None, output_name: Path = None):
        """This function mimics an xcore approach to convert from a raw8 image to rgb2.
        Kernels and Operations are all in float for simplicity.
        Kernel weights and saturations needs to be adjusted accordingly if implemented in fix point.

        The algorith is basically a channel split, with a 4x8 block processing.
        To avoid artifacts, further kernel logic could be implemented.

        Args:
            input_name (Path, optional): input file name. Defaults to None.
            output_name (Path, optional): output file name. Defaults to None.

        Returns:
            Pillow Image: returns demosaiced image.
        """
        kernels = kernel_array_rgb2
        self.check_kernels(kernels)
        img = self._imgread(input_name)
        out_size = (self.height // 2, self.width // 2, 3)
        img_out = np.zeros(out_size, dtype=np.float32).flatten()
        row_len = (self.width // 2) * 3  # row length in rgb
        for j in range(0, self.height, 4):
            rgb_ypos = (j // 2) * row_len  # this is ptr_out height location
            for i in range(0, self.width, 8):
                rgb_xpos = (i // 2) * 3  # this is ptr_out width location
                rgb_pos1 = rgb_ypos + rgb_xpos  # this is ptr_out position for first row
                block_4x8 = img[j : j + 4, i : i + 8, 0].astype(np.float32).flatten()
                for x in range(0, 24):
                    row_start = rgb_pos1 if x < 12 else (rgb_pos1 + row_len - 12)
                    img_out[row_start + x] = np.dot(block_4x8, kernels[x].flatten())

        img_out = img_out.reshape(out_size)
        img_out_pil = self._imgsave(img_out, input_name, output_name)
        return img_out_pil

    def raw8_to_rgb4_xcore(self, input_name: Path = None, output_name: Path = None):
        """This function mimics an xcore approach to convert from a raw8 image to rgb4.
        It produces a bilinear interpolation of 2x2 pixels from raw8 to rgb space.
        Kernels and Operations are all in float for simplicity.
        Kernel weights and saturations needs to be adjusted accordingly if implemented in fix point.

        The algorith is basically a channel split, with a 4x8 block processing.
        To avoid artifacts, further kernel logic could be implemented.

        Args:
            input_name (Path, optional): input file name. Defaults to None.
            output_name (Path, optional): output file name. Defaults to None.

        Returns:
            Pillow Image: returns demosaiced image.
        """
        kernels = kernel_array_rgb4
        self.check_kernels(kernels)
        img = self._imgread(input_name)
        out_size = (self.height // 4, self.width // 4, 3)
        img_out = np.zeros(out_size, dtype=np.float32).flatten()
        row_len = (self.width // 4) * 3  # row length in rgb
        for j in range(0, self.height, 4):
            rgb_ypos = (j // 4) * row_len  # this is ptr_out height location
            for i in range(0, self.width, 8):
                rgb_xpos = (i // 4) * 3  # this is ptr_out width location
                rgb_pos1 = rgb_ypos + rgb_xpos  # this is ptr_out position for first row
                block_4x8 = img[j : j + 4, i : i + 8, 0].astype(np.float32).flatten()
                for x in range(0, 6):
                    img_out[rgb_pos1 + x] = np.dot(block_4x8, kernels[x].flatten())

        img_out = img_out.reshape(out_size)
        img_out_pil = self._imgsave(img_out, input_name, output_name)
        return img_out_pil
    


    # ------------------ RGB ------------------
    def rgb_to_png(self, input_name=None, output_name=None):
        assert self.channels == 3, "This method is only for RGB images"
        img = self._imgread(input_name)
        img_pil = Image.fromarray(img)
        if output_name is None:
            output_name = Path(input_name).with_suffix(".png")
        img_pil.save(output_name)
        print("Image saved in:", output_name)
        self.last_img = img_pil
        return img_pil
    
    def rgb_apply_static_wb(self, _img: np.ndarray):
        assert(_img.dtype == np.int8), "Image must be int8"
        img = _img.copy().flatten()
        wb = np.array([1.538, 1.0, 1.587])
        pos = 0
        for i, px in enumerate(img):
            tmp = (wb[pos] * (px + 128)) - 127
            tmp = tmp.clip(-128, 127)
            tmp = tmp.astype(np.int8)
            img[i] = tmp
            pos = (pos + 1) % 3
        return img

    def rgb_apply_static_wb_uint8(self, _img: np.ndarray):
        assert(_img.dtype == np.uint8), "Image must be uint8"
        img = _img.copy().flatten()
        wb = np.array([1.538, 1.0, 1.587])
        pos = 0
        uint8_range = np.iinfo(np.uint8)
        for i, px in enumerate(img):
            tmp = (wb[pos] * (px))
            tmp = tmp.clip(uint8_range.min, uint8_range.max)
            tmp = tmp.astype(np.uint8)
            img[i] = tmp
            pos = (pos + 1) % 3
        img = img.reshape(_img.shape)
        return img
    
    # ------------------ YUV ------------------
    def yuv422_to_png(self, input_name=None, output_name=None):
        assert self.channels == 2, "This method is only for YUV images"
        img = self._imgread(input_name)
        img_pil = Image.fromarray(img)
        if output_name is None:
            output_name = Path(input_name).with_suffix(".png")
        img_pil.save(output_name)
        print("Image saved in:", output_name)
        self.last_img = img_pil
        return img_pil
    
    def yuv422_to_rgb_png(self, input_name=None, output_name=None):
        assert self.channels == 2, "This method is only for YUV images"
        img = self._imgread(input_name)
        img = cv2.cvtColor(img, cv2.COLOR_YUV2RGB_YUY2)
        img_pil = Image.fromarray(img)
        return self._imgsave(img, input_name, output_name)
    
    # ------------------ PLOT ------------------
    def plot(self, title=""):
        assert self.last_img is not None, "No image to plot"
        plt.figure()
        plt.imshow(self.last_img)
        plt.axis("off")  # Optional: to turn off axis labels
        plt.title(title)
        plt.show()


class ImageMetrics(object):
    def __init__(self):
        self.prec = 3  # precision for scores
        self.ssim_tol = 0.90
        self.psnr_tol = 20.0
        self.perc_error_tol = 25.0
        self.ratio_tol = 0.8
        self.rmse_tol = 4.0

    def ssim(self, img_ref, img):
        img_ref = np.array(img_ref.convert("L"))
        img = np.array(img.convert("L"))
        assert img_ref.shape == img.shape
        score = structural_similarity(img_ref, img)
        return np.round(score, self.prec)

    def psnr(self, img_ref, img):
        img_ref = np.array(img_ref)
        img = np.array(img)
        if np.array_equal(img_ref, img):
            return 50.0
        score = peak_signal_noise_ratio(img_ref, img)
        return np.round(score, self.prec)
    
    def mse(self, arr1, arr2):
        """Mean Squared Error"""
        assert arr1.shape == arr2.shape, "Arrays must have the same shape"
        mse = np.mean((arr1 - arr2) ** 2)
        mse = np.round(mse, self.prec)
        return mse
    
    def rmse(self, arr1, arr2):
        """Root Mean Squared Error"""
        assert arr1.shape == arr2.shape, "Arrays must have the same shape"
        rmse = np.sqrt(np.mean((arr1 - arr2) ** 2))
        rmse = np.round(rmse, self.prec)
        return rmse

    def get_metric(
        self,
        ref_name: str,
        ref: Image,
        img_name: str,
        img: Image,
        check=True,
        mprint=False,
    ):
        ssim = self.ssim(ref, img)
        psnr = self.psnr(ref, img)
        d = {"ref": ref_name, "img": img_name, "ssim": ssim, "psnr": psnr}
        if check:
            self.assert_metric(d)
        if mprint:
            print(d)
        return d

    def assert_metric(self, m):
        err_txt = f"{m}:\n \
            ssim_tol:{self.ssim_tol}, psnr_tol:{self.psnr_tol}"
        assert m["ssim"] > self.ssim_tol, err_txt
        assert m["psnr"] > self.psnr_tol, err_txt

    def get_perc_errors(self, metrics1, metrics2):
        """gets percentage errors between two metrics"""
        ssim1, ssim2 = metrics1["ssim"], metrics2["ssim"]
        psnr1, psnr2 = metrics1["psnr"], metrics2["psnr"]
        pe_ssim = (np.abs(ssim1 - ssim2) / ssim1) * 100
        pe_psnr = (np.abs(psnr1 - psnr2) / psnr1) * 100
        return pe_ssim, pe_psnr

    def get_ratios(self, metrics1, metrics2):
        """gets ratios between two metrics"""
        ssim1, ssim2 = metrics1["ssim"], metrics2["ssim"]
        psnr1, psnr2 = metrics1["psnr"], metrics2["psnr"]
        ratio_ssim = ssim2 / (ssim1 + 1e-6)
        ratio_psnr = psnr2 / (psnr1 + 1e-6)
        return np.round(ratio_ssim, 2), np.round(ratio_psnr, 2)

    def get_cross_metrics(self, metrics1, metrics2, check=True):
        """gets cross metrics between two metrics
        if check is True, it will assert the results based on the tolerances"""
        pe_ssim, pe_psnr = self.get_perc_errors(metrics1, metrics2)
        ratio_ssim, ratio_psnr = self.get_ratios(metrics1, metrics2)
        if check:
            assert pe_ssim < self.perc_error_tol, f"SSIM perc error is {pe_ssim}"
            assert pe_psnr < self.perc_error_tol, f"PSNR perc error is {pe_psnr}"
            assert ratio_ssim > self.ratio_tol, f"SSIM ratio is {ratio_ssim}"
            assert ratio_psnr > self.ratio_tol, f"PSNR ratio is {ratio_psnr}"


if __name__ == "__main__":
    raw_in = folder_in / "capture0_int8.raw"
    input_size = ImgSize(height=200, width=200, channels=1, dtype=np.int8)
    img_decoder = ImageDecoder(input_size)
    img_decoder.raw8_to_rgb1(raw_in)
    img_decoder.plot()
