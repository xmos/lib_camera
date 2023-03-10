# Camera framework
Repository to manipulate cameras using the XCORE.AI sensor

# To use first update dependencies
west update

# usefull commands
- build : ```sh launch_cmake.sh```
- run (explorer board):  ```xrun --xscope example_take_picture.xe```
- run (simulation):  ```xsim --xscope "-offline trace.xmt" build/example_take_picture.xe```

# Supported Cameras

| Model  | Max Resolution | Output Formats
| ------------- | ------------- | ------------- |
| IMX219 | 640Hx480V == VGA (2x2 binning) | RAW10
| GC2145 | 1600H x 1200V == 2MPX | YUV422
