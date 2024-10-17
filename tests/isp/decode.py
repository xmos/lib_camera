import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from PIL import Image  # To avoid color BGR issues when writing

cwd = Path(__file__).parent.absolute()
path_imgs = cwd / "src" / "imgs"

folder_in = path_imgs
folder_out = path_imgs
assert folder_in.exists(), f"Folder {folder_in} does not exist"

def decode(input_name=None, output_name=None, plot=False):
    height = 200
    width = 200
    with open(input_name, "rb") as f:
        data = f.read()

    buffer = np.frombuffer(data, dtype=np.int8)
    buffer = buffer.astype(np.int16) + 128
    buffer = buffer.astype(np.uint8)
    img = buffer.reshape(height, width, 3).astype(np.uint8)

    # Convert the numpy array to a PIL image and then to a Matplotlib plot
    img_pil = Image.fromarray(img)
    img_pil.save(output_name)
    
    if plot:
        plt.figure()
        plt.imshow(img)
        plt.axis("off")  # Optional: to turn off axis labels
        plt.show()


if __name__ == "__main__":
    input_name = folder_in / "capture_int8.rgb"
    output_name = folder_out / "capture_int8.png"
    decode(input_name=input_name, output_name=output_name, plot=True)
