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

.. code-block:: console
    
    cmake -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake -B build
    make -C build tests

.. code-block:: console

    cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake -B build
    ninja -C build tests


Running the tests
=================

.. note::
  1. Hardware tests require `xscope_fileio` to be installed.
  2. Run the following commands from the `fwk_camera` top level.

Run unit tests
--------------

.. code-block:: bash

   xsim --xscope "-offline trace.xmt" build/tests/unit_tests/test_camera.xe

Run hardware tests
------------------

.. code-block:: bash

   pytest
