.. _QS_FWKC:

Quick Start Guide
-------------------

This document will provide a quick tour through the ``fwk_camera`` repository. It will go through the process
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

Make sure all submodules are imported: 

.. code-block:: console

  git clone --recurse-submodules https://github.com/xmos/fwk_camera.git

Run the RAW camera demo
^^^^^^^^^^^^^^^^^^^^^^^
This demo uses the RAW camera module to capture a RAW8 image and save it to a .raw file. 
Then, this image can be decoded using the python script ``python decode_raw8.py``.

1. Ensure that the camera is connected to the board.
2. Connect Power Supply and xTag debugger.
3. Install python requirements. Inside a virtual environment, install the python requirements using the command:

  .. code-block:: console
    
    pip install -r requirements.txt

4. Build the example using the following commands:

  .. tab:: Linux and Mac

    .. code-block:: console
    
      >> Linux and Mac
      cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
      make -C build example_take_picture_raw

  .. tab:: Windows

    .. code-block:: console

      >> Windows
      cmake -G Ninja -B build --toolchain=xmos_cmake_toolchain\xs3a.cmake
      ninja -C build example_take_picture_raw
      cd utils/xscope_fileio/host # Windows requires some extra steps
      cmake -G Ninja . && ninja

5. Run the example using the following command:

.. code-block:: console       

  python python/run_xscope_bin.py \
    build/examples/take_picture_raw/example_take_picture_raw.xe

6. The camera should communicate with the host and save the image to a .raw file

7. To decode the image use the following command:

  .. code-block:: console     

    python python/decode_raw8.py

8. The decoded image should be displayed on the screen
