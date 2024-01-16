Camera framework
==================

This repository serves as a comprehensive software solution for camera manipulation using the XCORE.AI sensor.

Repository Structure
--------------------

- **examples**    : examples for taking pictures with the explorer board
- **camera**      : useful functions to manipulate images
- **modules**     : dependencies folder
- **sensors**     : configuration and control of the sensors
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

.. code-block:: console

  git clone --recurse-submodules https://github.com/xmos/fwk_camera.git

Build instructions
------------------

The instructions below will build all modules, examples and tests.
For building a specific example refer to examples/readme.rst.

Linux, Mac
~~~~~~~~~~

.. code-block:: console

  cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
  make -C build


Windows
~~~~~~~

.. code-block:: console

  cmake -G Ninja -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
  ninja -C build


Installing xscope_fileio host tools
-----------------------------------

To install ``xscope_fileio`` host tool, please follow the steps below:

1. Make sure you have a C compiler  installed. If you are developing on Windows, we recomend using VS tools with a ``cl`` compiler.
2. Open a terminal or command prompt.
3. Install the xscope_fileio package:

.. tab:: Linux and Mac

  .. code-block:: console

    >> Linux and Mac
    pip install -e ../xscope_fileio

.. tab:: Windows

  .. code-block:: console

    >> Windows
    pip install -e ../xscope_fileio
    cmake -G Ninja -S ../xscope_fileio/host -B ../xscope_fileio/host
    ninja -C ../xscope_fileio/host
  
Your ``xscope_fileio`` host app is now ready to use.
