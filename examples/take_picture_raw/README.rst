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

.. tab:: Linux and Mac

    .. code-block:: console
        
        cmake -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake -B build
        make -C build example_take_picture_raw

.. tab:: Windows

    .. code-block:: console

        cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake -B build
        ninja -C build example_take_picture_raw

***************
Running example
***************

From the top level

.. tab:: Linux and Mac

    .. code-block:: console

        pip install -e utils/xscope_fileio
        python python/run_xscope_bin.py build/examples/take_picture_raw/example_take_picture_raw.xe

.. tab:: Windows

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
