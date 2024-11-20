# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import cv2

import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from PIL import Image  # To avoid color BGR issues when writing
from skimage.metrics import peak_signal_noise_ratio
from skimage.metrics import structural_similarity

cwd = Path(__file__).parent.absolute()
path_imgs = cwd / "src" / "imgs"

folder_in = path_imgs
folder_out = path_imgs
assert folder_in.exists(), f"Folder {folder_in} does not exist"


# TODO this classes will eventually be part of the library at some point
# and will replace all small and independent python functions in "/python" folder
class ImageDecoder(object):
    def __init__(self, height=200, width=200, channels=None, dtype=np.int8, mode=None):
        self.height = height
        self.width = width
        self.channels = channels
        self.dtype = dtype
        self.last_img = None
        self.modes = ["raw8", "rgb"]

        if mode is not None:
            self.set_mode(mode)

    def _imgread(self, input_name):
        with open(input_name, "rb") as f:
            data = f.read()
        buffer = np.frombuffer(data, dtype=self.dtype)

        if self.dtype == np.int8:
            buffer = buffer.astype(np.int16) + 128

        img = buffer.reshape(self.height, self.width, self.channels).astype(np.uint8)
        return img

    def set_mode(self, mode):
        assert mode in self.modes
        self.channels = 1 if mode == "raw8" else 3 if mode == "rgb" else None

    def decode_raw8(self, input_name=None, output_name=None):
        assert self.channels == 1, "This method is only for raw images"
        img = self._imgread(input_name)
        img = cv2.cvtColor(img, cv2.COLOR_BayerBG2RGB)  # we assum RG GB .. pattern
        img_pil = Image.fromarray(img)
        if output_name is None:
            output_name = Path(input_name).with_suffix(".png")
        img_pil.save(output_name)
        print("Image saved in:", output_name)
        self.last_img = img_pil
        return img_pil

    def decode_rgb(self, input_name=None, output_name=None):
        assert self.channels == 3, "This method is only for RGB images"
        img = self._imgread(input_name)
        img_pil = Image.fromarray(img)
        if output_name is None:
            output_name = Path(input_name).with_suffix(".png")
        img_pil.save(output_name)
        print("Image saved in:", output_name)
        self.last_img = img_pil
        return img_pil

    def plot(self, title=""):
        assert self.last_img is not None, "No image to plot"
        plt.figure()
        plt.imshow(self.last_img)
        plt.axis("off")  # Optional: to turn off axis labels
        plt.title(title)
        plt.show()


class ImageMetrics(object):
    def __init__(self):
        self.prec = 2  # precision for scores
        self.ssim_tol = 0.90
        self.psnr_tol = 20.0

    def ssim(self, img_ref, img):
        img_ref = np.array(img_ref.convert("L"))
        img = np.array(img.convert("L"))
        assert img_ref.shape == img.shape
        score = structural_similarity(img_ref, img)
        return np.round(score, self.prec)

    def psnr(self, img_ref, img):
        img_ref = np.array(img_ref)
        img = np.array(img)
        score = peak_signal_noise_ratio(img_ref, img)
        return np.round(score, self.prec)

    def get_metric(self, img_ref, img, idx=None, check=False, mprint=False):
        ssim = self.ssim(img_ref, img)
        psnr = self.psnr(img_ref, img)
        d = {"name": idx, "ssim": ssim, "psnr": psnr}
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


if __name__ == "__main__":
    rgb_in = folder_in / "capture0_int8.rgb"
    raw_in = folder_in / "capture0_int8.raw"

    dec0 = ImageDecoder(mode="rgb")
    dec1 = ImageDecoder(mode="raw8")
    met = ImageMetrics()

    dec1.decode_raw8(raw_in)
    dec0.decode_rgb(rgb_in)
    met.get_metric(dec0.last_img, dec1.last_img, "test", mprint=True)
