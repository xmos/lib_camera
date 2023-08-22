Building the Software
=======================

.. include:: ../substitutions.rst

This section will provide details on how the software is constructed. The basic steps and build requirements
can be found in the README.rst file which is distributed with the source.


Requirements
------------

Hardware requirements:
^^^^^^^^^^^^^^^^^^^^^^^
- XCORE.AI EVALUATION KIT (XK-EVK-XU316)
- Camera module
- Camera ribbon connector
- 2x Micro USB cable (Power supply and xTag)
- xTag debugger and cable

Software requirements:
^^^^^^^^^^^^^^^^^^^^^^^
- XMOS tools (15.2.1): `SW_TOOLS`_
- CMake, Ninja (Windows)
- Python 3.7 or later

Bulding the firmware and the examples
-------------------------------------

``fwk_camera`` is intended to be used as part of an application, therefore depending if you have it on its own or not, you will
be able to build examples. By following the instructions below, you'll be able to build all possible targets.

.. tab:: Linux and Mac

  .. code-block:: console

    cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
    make -C build

.. tab:: Windows

  .. code-block:: console

    cmake -G Ninja -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
    ninja -C build

Building the host app (xscope_fileio)
-------------------------------------

All ``fwk_camera`` examples do ``fileio`` over a tool called ``xscope_fileio``. There is a python wrapper script ``python/run_xcore_bin.py``
which runs will run the example binary of your choice using ``xscope_fileio``. This script reqires some ``xscope_fileio``
host binaries. You can find the installation instructions in this :ref:`page<xscope_fileio_instructions>`. After you've gone
through those instructions once, all you'll need to do is activate your python virtual environment.

Running the examples
--------------------

1. Make sure you have activated your python environment with installed ``xscope_fileio``.
2. Run the example with the following command:

.. code-block:: console

  python python/run_xscope_bin.py <path to your example binary>
  # or
  python python/run_xscope_bin.py # and chose a binary in the dropdown menu
