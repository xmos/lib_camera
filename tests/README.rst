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

Linux, Mac
----------

.. code-block:: console
  
  cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
  make -C build tests

Windows
-------

.. code-block:: console

  cmake -G "Ninja" -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
  ninja -C build tests

Running the tests
=================

.. note::
  1. Hardware tests require `xscope_fileio` to be installed.
  2. Run the following commands from the `fwk_camera` top level.

Run unit tests
--------------

.. code-block:: console

  xsim --xscope "-offline trace.xmt" build/tests/unit_tests/test_camera.xe
  # or
  xrun --xscope build/tests/unit_tests/test_camera.xe

Run hardware tests
------------------

.. code-block:: console

   pytest
