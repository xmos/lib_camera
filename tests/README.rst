================================
Test Folder
================================

This folder contains various types of tests for the project.
It contains two types of tests:
  1. Unit tests
  2. Hardware tests

Build Tests
=============

Run the following commands from the current folder:

.. code-block:: console

  cmake -G "Unix Makefiles" -B build
  xmake -C build

Run unit tests (xrun or xsim)
-----------------------------

Run the following commands from the ``unit_tests`` folder:

.. code-block:: console

  # Simulate the test
  xsim --xscope "-offline trace.xmt" bin/test_camera.xe
  # Run the test on hardware
  xrun --xscope bin/test_camera.xe

Run hardware tests
------------------

.. note::
  
  Hardware tests require `xscope_fileio` to be installed.

Run the following commands from the ``hardware_tests`` folder:

.. code-block:: console

  pytest
