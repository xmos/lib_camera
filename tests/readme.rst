================================
Test Folder
================================

This folder contains various types of tests for the project.
It contains two types of tests:
    1. Unit tests
    2. Hardware tests


Build Tests
=============

Run the following commands from the top level:

.. tabs::
    .. tab:: Linux and Mac

        .. code-block:: console
            
            cmake -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake -B build
            make -C build tests

    .. tab:: Windows

        .. code-block:: console

            cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake -B build
            ninja -C build tests


Running the tests
=============
* hardware tests require xscope_fileio installed
