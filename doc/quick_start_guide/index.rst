.. _QS_FWKC:

Quick Start Guide
-------------------

This document will provide a quick tour through the |lib_camera| repository. It will go through the process
of building the example application and taking a photo in RAW8 format.

Hardware requirements
^^^^^^^^^^^^^^^^^^^^^
- xcore.ai evaluation kit (XK-EVK-XU316)
- Camera module
- Camera ribbon connector
- 2x Micro USB cable (Power supply and xTag)
- xTag debugger and cable

Software requirements
^^^^^^^^^^^^^^^^^^^^^
- XTC tools (15.2.1): `SW_TOOLS`_
- CMake, Ninja (Windows)
- Python 3.9 or later 

Sandbox Structure
^^^^^^^^^^^^^^^^^
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
^^^^^^^^^^^^
Clone the following repositories using the commands:

.. code-block:: console

  git clone https://github.com/xmos/lib_camera.git
  git clone https://github.com/xmos/xscope_fileio.git ../xscope_fileio
  git clone https://github.com/xmos/xcommon_cmake.git ../xcommon_cmake

Install a Python virtual environment and install the Python dependencies:

.. code-block:: console

  pip install -r requirements.txt
  pip install -e ../xscope_fileio

Xcommon cmake setup
^^^^^^^^^^^^^^^^^^^
This repository uses ``xcommon_cmake`` and ``xmake`` as a build system. 

- ``xcommon_cmake`` is a collection of CMake functions and macros that are used to build XCORE.AI projects. `xcommon_cmake/doc <https://github.com/xmos/xcommon_cmake/tree/develop/doc>`_. 

- ``xmake`` is a fork of make, native to XTC tools. `xmake-manual <https://www.xmos.com/documentation/XM-014363-PC-4/html/tools-guide/tools-ref/cmd-line-tools/xmake-manual/xmake-manual.html>`_.  

In order to build the examples, you need to set the ``XCOMMON_CMAKE_PATH`` environment variable to the path of the ``xcommon_cmake`` repository. 
For example for Windows:

.. code-block:: console

  set XMOS_CMAKE_PATH=C:/Users/user_x/sandbox/xcommon_cmake

Xscope fileio setup (Windows)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On Windows the host app of ``xscope_fileio`` is not installed by default, please follow the steps below:

1. Make sure you have a C compiler installed. We recommend using VS tools with a ``cl`` compiler.
2. Open a terminal or command prompt.
3. Build the host app using the following commands:

.. code-block:: console

    cmake -G Ninja -S ../xscope_fileio/host -B ../xscope_fileio/host
    ninja -C ../xscope_fileio/host
  
Your ``xscope_fileio`` host app is now ready to use.

Build instructions
^^^^^^^^^^^^^^^^^^

In order to build any example, go to the example that you want to build and follow the instructions in the ``README.rst`` file.
Alternatively, you can build all the examples using the following command:

.. code-block:: console

  python examples/build_examples.py

Below we demonstrate how to build and run the take picture RAW camera demo.

Building and Running the RAW camera demo
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This demo uses the RAW camera module to capture a RAW8 image and save it to a .raw file. 
Then, this image can be decoded using the Python script ``python decode_raw8.py``.

1. Ensure that the camera is connected to the board.
2. Connect the Power Supply and xTag debugger.
3. Build the example using the following command inside the ``examples/take_picure_raw`` folder:
   
  .. code-block:: console       

    cmake -G "Unix Makefiles" -B build
    xmake -C build

4. Run the example using the following command in the example folder:

  .. code-block:: console

    python python/run_xscope_bin.py \
    examples/take_picture_raw/bin/take_picture_raw.xe

5. The camera should communicate with the host and save the image to a .raw file

6. To decode the image use the following command:

  .. code-block:: console

    python python/decode_raw8.py

7. The decoded image should be displayed on the screen
