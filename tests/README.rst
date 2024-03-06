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

Running the tests
=================

.. note::
  1. Hardware tests require `xscope_fileio` to be installed.
  2. Run the following commands from the `lib_camera` top level.

Run unit tests (xrun or xsim)
-----------------------------

.. code-block:: console

  xsim --xscope "-offline trace.xmt" bin/test_camera.xe
  xrun --xscope bin/test_camera.xe

Run hardware tests
------------------

.. code-block:: console

   pytest
