# Copyright 2024-2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import cv2

import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from PIL import Image  # To avoid color BGR issues when writing
from skimage.metrics import peak_signal_noise_ratio
from skimage.metrics import structural_similarity

from kernels import kernel_array_rgb2

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

    def set_mode(self, mode):
        assert mode in self.modes
        self.channels = 1 if mode == "raw8" else 3 if mode == "rgb" else None

    def raw8_to_rgbx(self, input_name=None, output_name=None, k_factor=2):
        img = self._imgread(input_name)
        img = cv2.cvtColor(img, cv2.COLOR_BayerBG2RGB)
        img_pil = Image.fromarray(img)
        if k_factor > 1:
            img_pil = img_pil.resize((self.width // k_factor, self.height // k_factor))
        if output_name is None:
            output_name = Path(input_name).with_suffix(".png")
        img_pil.save(output_name)
        print("Image saved in:", output_name)
        self.last_img = img_pil
        return img_pil

    def raw8_read(self, input_name):
        img = self._imgread(input_name)
        return img

    def raw8_to_rgb1(self, input_name=None, output_name=None):
        return self.raw8_to_rgbx(input_name, output_name, 1)

    def raw8_to_rgb2(self, input_name=None, output_name=None):
        return self.raw8_to_rgbx(input_name, output_name, 2)

    def check_kernels(self, kernel_arr):
        for kernel in kernel_arr:
            assert kernel.sum() == 1.0, "Kernel does not sum to 1.0"
        print("All kernels are valid")

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
                    img_out[row_start + x] = np.dot(block_4x8, kernels[x])

        img_out = img_out.reshape(out_size)
        img_out_pil = self._imgsave(img_out, input_name, output_name)
        return img_out_pil

    def raw8_to_rgb4(self, input_name=None, output_name=None):
        return self.raw8_to_rgbx(input_name, output_name, 4)

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

    def generate_pure_raw(self, color="green", out_filename=None):
        raw_img = np.zeros((self.height, self.width, 1), dtype=np.int8)
        if color == "red":
            raw_img[0::2, 0::2] = np.iinfo(np.int8).max
            raw_img[0::2, 1::2] = np.iinfo(np.int8).min
            raw_img[1::2, 0::2] = np.iinfo(np.int8).min
            raw_img[1::2, 1::2] = np.iinfo(np.int8).min
        elif color == "green":
            raw_img[0::2, 0::2] = np.iinfo(np.int8).min
            raw_img[0::2, 1::2] = np.iinfo(np.int8).max
            raw_img[1::2, 0::2] = np.iinfo(np.int8).max
            raw_img[1::2, 1::2] = np.iinfo(np.int8).min
        elif color == "blue":
            raw_img[0::2, 0::2] = np.iinfo(np.int8).min
            raw_img[0::2, 1::2] = np.iinfo(np.int8).min
            raw_img[1::2, 0::2] = np.iinfo(np.int8).min
            raw_img[1::2, 1::2] = np.iinfo(np.int8).max
        else:
            raise ValueError("Invalid color choice")
        with open(out_filename, "wb") as img:
            img.write(raw_img)
        return raw_img


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


if __name__ == "__main__":
    rgb_in = folder_in / "capture0_int8.rgb"
    raw_in = folder_in / "capture0_int8.raw"

    dec0 = ImageDecoder(mode="rgb")
    dec1 = ImageDecoder(mode="raw8")
    met = ImageMetrics()

    dec1.raw8_to_rgb1(raw_in)
    dec0.decode_rgb(rgb_in)
    met.get_metric(dec0.last_img, dec1.last_img, "test", mprint=True)
