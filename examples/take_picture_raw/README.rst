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

Linux and Mac
~~~~~~~~~~~~~

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

Make sure you have XTC tools environment activated and run the following commands from the top level:

Linux and Mac
~~~~~~~~~~~~~

.. code-block:: console

    pip install -e utils/xscope_fileio
    python python/run_xscope_bin.py build/examples/take_picture_raw/example_take_picture_raw.xe

Windows
~~~~~~~

.. code-block:: console

    # works with a cl compiler
    pip install -e utils/xscope_fileio
    cd utils/xscope_fileio/host
    cmake -G "Ninja" . && ninja
    cd ../../../
    python python/run_xscope_bin.py build/examples/take_picture_raw/example_take_picture_raw.xe


******
Output
******

The output file ``capture.bin`` will be generated at the top level the repository. It can be further processed using ``python/decode_raw8.py`` script.
