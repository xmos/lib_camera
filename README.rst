Camera framework
==================

This repository serves as a comprehensive software solution for camera manipulation using the XCORE.AI sensor.

Repository Structure
--------------------

- **examples**    : examples for taking pictures with the explorer board
- **lib_camera**  : useful functions to manipulate images
- **modules**     : dependencies folder
- **sensors**     : camera sensors and API for controlling any camera sensor
- **python**      : python functions to decode RAW8, RAW10 pictures and other utilities to treat images

Requirements
------------

- CMAKE
- XMOS tools
- git submodules 
- Ninja (Windows)

Installation
------------

Some dependent components are included as git submodules. These can be obtained by cloning this repository with the following command:
(make sure you have the correct ssh access to clone)

.. code-block:: bash

   git clone --recurse-submodules git@github.com:xmos/fwk_camera.git

Build instructions
------------------

The instructions below will build all modules, examples and tests.
For building a specific example refer to examples/readme.rst.

Linux, Mac
~~~~~~~~~~

.. code-block:: bash

   cmake -B build -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake
   cd build/
   make

Windows
~~~~~~~

.. code-block:: bash

   cmake -G Ninja -B build -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake
   cd build/
   ninja

Supported Cameras
-----------------

+--------+--------------------------------+----------------+
| Model  | Max Resolution                 | Output Formats |
+========+================================+================+
| IMX219 | 640Hx480V == VGA (2x2 binning) | RAW8           |
+--------+--------------------------------+----------------+
| IMX219 | 160x120x3 RGB                  | RGB            |
+--------+--------------------------------+----------------+
| GC2145 | 1600H x 1200V == 2MPX          | YUV422         |
+--------+--------------------------------+----------------+

How to configure your sensor or add a new one
--------------------------------------------

TODO
