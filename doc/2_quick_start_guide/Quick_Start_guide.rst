.. _QS_FWKC:

Quick Start Guide
-------------------

.. include:: ../substitutions.rst

Hardware requirements:
^^^^^^^^^^^^^^^^^^^^^^^
- XCORE.AI EVALUATION KIT (XK-EVK-XU316)
- Camera module
- Camera ribbon connector
- 2x Micro USB cable (Power supply and xTag)
- xTag debugger and cable

Software requirements:
^^^^^^^^^^^^^^^^^^^^^^^
- XMOS tools (15.2.1): `SW_TOOLS`_
- CMake, Ninja (Windows)
- Python 3.7 or later 

Make sure all submodules are imported: 

.. code-block:: console

   git clone --recurse-submodules git@github.com:xmos/fwk_camera.git

Run the RAW camera demo
^^^^^^^^^^^^^^^^^^^^^^^
This demo uses the RAW camera module to capture a RAW8 image and save it to a .raw file. 
Then, this image can be decoded using the python script ``python decode_raw8.py``.

.. warning::
  Make sure xscope_fileio is installed. Refer to :ref:`xscope_fileio_instructions` page for more information.

1. Ensure that the camera is connected to the board
2. Connect Power Supply and xTag debugger
3. Install python requirements. From the root of the repository run:

.. code-block:: console
  
  pip install -r requirements.txt

4. Build the example using the following commands:

.. tab:: Linux and Mac

  .. code-block:: console
  
    cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
    make -C build example_take_picture_raw

.. tab:: Windows

  .. code-block:: console

    cmake -G Ninja -B build --toolchain=xmos_cmake_toolchain\xs3a.cmake
    ninja -C build example_take_picture_raw


5. Run the example using the following command:

.. code-block:: console       

  python python/run_xscope_bin.py build/examples/take_picture_raw/example_take_picture_raw.xe

6. You should see the camera communicating with the host and the image being saved to a .raw file.

7. To decode the image use the following command:

.. code-block:: console     

  python python/decode_raw8.py

8. You should see the decoded image displayed on the screen
