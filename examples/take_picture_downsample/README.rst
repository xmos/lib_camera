================================
Example: Take picture downsample
================================

This example set the basic settings for the sony sensor and grab a single frame. 
By default the format is the following:
- 160x120x3 RGB

*************
Build example
*************
Run the following commands from the top level:

Linux, Mac
~~~~~~~~~~

.. code-block:: console
        
        cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
        make -C build example_take_picture_downsample

Windows
~~~~~~~

.. code-block:: console

        cmake -G "Ninja" -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
        ninja -C build example_take_picture_downsample

***************
Running example
***************

From the top level. 
Make sure ``xscope_fileio`` is installed. See /utils/README.rst section for more details.

.. code-block:: console

    python python/run_xscope_bin.py build/examples/take_picture_downsample/example_take_picture_downsample.xe

******
Output
******

The output files ``capture.bin`` and ``capture.bmp`` will be generated at the top level the repository. ``capture.bin`` can be further processed using ``python/decode_downsampled.py`` script.
