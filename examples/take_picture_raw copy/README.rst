=========================
Example: Take picture RAW
=========================

This example set the basic settings for the sony sensor and grab a single frame. 
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
        make -C build example_take_picture_raw

Windows
~~~~~~~

.. code-block:: console

        cmake -G "Ninja" -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
        ninja -C build example_take_picture_raw

***************
Running example
***************

From the top level
Make sure ``xscope_fileio`` is installed. See /utils/README.rst section for more details.

.. code-block:: console
    
    python python/run_xscope_bin.py build/examples/take_picture_raw/example_take_picture_raw.xe


******
Output
******

The output file ``capture.bin`` will be generated at the top level the repository. It can be further processed using ``python/decode_raw8.py`` script.
