# Camera framework
Repository to manipulate cameras using the XCORE.AI sensor

## Structure
- **examples**    : examples for taking pictures with the explorer board
- **lib_camera**  : useful functions to manipulate images
- **modules**     : dependencies folder
- **sensors**     : camera sensors and API for controlling any camera sensor
- **python**      : python functions to decode RAW8, RAW10 pictures and other utilities to treat images

## Requirements
- CMAKE
- XMOS tools
- git submodules 

## Installation
```
git clone 
cd ./fwk_camera
git submodule update --init --recursive
sh launch_cmake.sh
```

## Useful commands
- build : ```sh launch_cmake.sh```
- run (explorer board):  ```xrun --xscope example_take_picture.xe```
- run (simulation):  ```xsim --xscope "-offline trace.xmt" build/example_take_picture.xe```

## Supported Cameras

| Model  | Max Resolution | Output Formats
| ------------- | ------------- | ------------- |
| IMX219 | 640Hx480V == VGA (2x2 binning) | RAW8
| GC2145 | 1600H x 1200V == 2MPX | YUV422

## How to configure your sensor or add a new one
TODO
