Camera Library
==============

This repository serves as a comprehensive software solution for camera manipulation using the XCORE.AI sensor.

Repository Structure
--------------------

- **doc**         : documentation for the camera library
- **examples**    : examples for taking pictures with the vision board
- **lib_camera**  : library to manipulate cameras
- **python**      : python functions to decode RAW8, RAW10 pictures and other utilities to treat images
- **tests**       : tests for the camera library

Examples
--------

- **take_picture_downsample** : takes a picture from an input size of 680x480 or 1280x960 and downsample it down to 160x120. 
- **take_picture_local**      : similar to ``take_picture_downsample`` but the MIPI input is a .raw image not the camera.
- **take_picture_raw**        : takes a raw8 680x480 picture and save it to a .raw file.


Quick Start
-----------

In order to setup the project and build the examples, please refer to the documentation:

`doc/quick_start_guide <./doc/quick_start_guide/quick_start_guide.rst>`_.
