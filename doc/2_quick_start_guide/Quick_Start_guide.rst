.. _QS_FWKC:

Quick Start Guide
-------------------

.. include:: ../substitutions.rst

Hardware requirements:
^^^^^^^^^^^^^^^^^^^^^^^
- XCORE.AI EVALUATION KIT (XK-EVK-XU316)
- Camera module
- Power supply
- Micro USB cable
- JTAG debugger

Software requirements:
^^^^^^^^^^^^^^^^^^^^^^^
- XMOS tools: `SW_TOOLS`_
- CMake, Ninja (Windows)
- Python 3.7 or later 
- Xscope_fileio

Make sure all submodules are imported: 

.. code-block:: console

   git clone --recurse-submodules git@github.com:xmos/fwk_camera.git

Run the RAW camera demo
^^^^^^^^^^^^^^^^^^^^^^^
This demo uses the RAW camera module to capture a RAW8 image and save it to a .raw file. 
Then, this image can be decoded using the python script ``python decode_RAW8.py``.

1. Make sure that the camera is connecte to the board
2. Connect Power Supply and JTAG debugger
3. Build the example using the following commands:


.. tab:: Linux and Mac

    .. code-block:: console
    
        cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
        make -C build example_take_picture_raw

.. tab:: Windows

    .. code-block:: console

        cmake -G Ninja -B build --toolchain=xmos_cmake_toolchain\xs3a.cmake
        ninja -C build example_take_picture_downsample


4. Run the example using the following command:

.. code-block:: console       

    python python/run_xscope_bin.py build/examples/take_picture_raw/example_take_picture_raw.xe

5. You should see the camera comminucating with the host and the image being saved to a .raw file
6. To decode the image use the following command:

.. code-block:: console     

    python python/decode_raw8.py

7. You should see the decoded image displayed on the screen
