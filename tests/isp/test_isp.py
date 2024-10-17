import matplotlib.pyplot as plt
import numpy as np
import subprocess
from pathlib import Path
from PIL import Image  # To avoid color BGR issues when writing

from string import Template 
from decode import decode

cwd = Path(__file__).parent.absolute()
imgs = cwd / "src" / "imgs"

cmake_template = Template(
    'cmake \
        -D FILE_IN_NAME="$file_in" \
        -D FILE_OUT_NAME="$file_out" \
        -G Ninja -B build --fresh'
)

build_cmd = "ninja -C build"
run_cmd = "python ../../python/run_xscope_bin.py bin/test_isp_rgb1.xe"

def test_isp():
    print("Testing ISP")
    img_glob = imgs.glob("*.raw")
    for file_in in img_glob:
        file_in_str = str(file_in.relative_to(cwd)).replace("\\", "/")
        file_out = file_in_str.replace(".raw", ".rgb")
        file_out_path = file_in.with_suffix(".rgb")
        file_out_png = file_out_path.with_suffix(".png")
        cmd = cmake_template.substitute(file_in=file_in_str, file_out=file_out)
        
        # cmake, build, run, decode
        print("=====================================================")
        print("Testing ISP for file:", file_in)
        subprocess.run(cmd, shell=True, cwd=cwd, check=True)
        subprocess.run(build_cmd, shell=True, cwd=cwd, check=True)
        subprocess.run(run_cmd, shell=True, cwd=cwd, check=True)
        decode(input_name=file_out_path, output_name=file_out_png, plot=False)
        

if __name__ == '__main__':
    test_isp()
