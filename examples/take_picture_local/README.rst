=========================
Example: Take picture Local
=========================

This example set the basic settings for inject a RAW image
to the explorer board an run the ISP pipeline. 

If you do not have a RAW8 image, you can produce it with 
the utility encode_raw8 from the python folder.

By default the format is the following:
- 640x480 RAW8

*************
Build example
*************
Run the following commands from the top level:

Linux, Mac
~~~~~~~~~~

.. code-block:: console
        
        cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
        make -C build example_take_picture_local

Windows
~~~~~~~

.. code-block:: console

        cmake -G "Ninja" -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
        ninja -C build example_take_picture_local

***************
Running example
***************

From the top level
Make sure ``xscope_fileio`` is installed. See /utils/README.rst section for more details.

.. code-block:: console
    
    python python/run_xscope_bin.py build/examples/take_picture_local/example_take_picture_local.xe


******
Output
******

The output files ``capture.bin`` and ``capture.bmp`` will be generated at the top level the repository. 
``capture.bin`` can be further processed using ``python/decode_downsampled.py`` script.
