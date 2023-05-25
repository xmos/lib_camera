## Python 
utilities for treating raw images

## File description:
* decode_raw8.py  : decode a raw8 binary image. Take care of choosing the right channel order. By default RGGB. 
* decode_raw10.py : decode a raw10 binary image. Take care of choosing the right channel order. By default RGGB. 
* FIR pipeline    : describe the process from a raw image to the pipeline that would be performed inside the xcore. 

## Environement
Set an .env file with the following variables inside:
BINARY_IMG_PATH : path to the binary image produced by the xcore (usually the folder where xrun was performed)
