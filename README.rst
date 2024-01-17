Camera framework
==================

This repository serves as a comprehensive software solution for camera manipulation using the XCORE.AI sensor.

Repository Structure
--------------------

- **doc**         : documentation for the camera library
- **examples**    : examples for taking pictures with the explorer board
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

[quick start guide](#quick_start_guide)


Requirements
------------

- XTC tools 15.2.1
- CMake 3.21 or later
- Python 3.9 or later 

Sandbox Structure
-----------------

This repository is structured as a sandbox. 
It will install dependencies above the current folder. So the folder structure should look like this:

.. code-block:: console

  sandbox
    |
    |--- lib_camera (current repository)
    |--- xscope_fileio
    |--- xcommon_cmake
    |--- other_libs...


Installation
------------

Clone the the following repositories using the commands:

.. code-block:: console

  git clone https://github.com/xmos/lib_camera.git
  git clone https://github.com/xmos/xscope_fileio.git ../xscope_fileio
  git clone https://github.com/xmos/xcommon_cmake.git ../xcommon_cmake

Install a python environement and install the python dependencies:

.. code-block:: console

  pip install -r requirements.txt
  pip install -e ../xscope_fileio

Xcommon cmake setup
-------------------

This repository uses ``xcommon_cmake`` and ``xmake`` as a build system. 

``xcommon_cmake`` is a collection of cmake functions and macros that are used to build XCORE.AI projects. more information here: https://github.com/xmos/xcommon_cmake/tree/develop/doc
``xmake`` is native to XTC tools. more information here: https://www.xmos.com/documentation/XM-014363-PC-4/html/tools-guide/tools-ref/cmd-line-tools/xmake-manual/xmake-manual.html

In order to build the examples, you need set the ``XCOMMON_CMAKE_PATH`` environment variable to the path of the ``xcommon_cmake`` repository. For example:

Xscope fileio setup (windows)
-----------------------------

To install ``xscope_fileio`` tool, please follow the steps below:

1. Make sure you have a C compiler  installed. We recomend using VS tools with a ``cl`` compiler.
2. Open a terminal or command prompt.

3. Install the xscope_fileio package:

.. code-block:: console

    cmake -G Ninja -S ../xscope_fileio/host -B ../xscope_fileio/host
    ninja -C ../xscope_fileio/host
  
Your ``xscope_fileio`` host app is now ready to use.

Build instructions
------------------

Go the example that you want to build and follow the instructions in the ``README.rst`` file.
Alternatively, you can build all the examples using the following command:

.. code-block:: console

  python examples/build_examples.py
