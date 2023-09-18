Building and running the Software
=================================

.. include:: ../substitutions.rst

This section will provide details on how the software can be build and run. It will go through the full process from
the requirements to the user seing images taked from xcore on the their screen.

Requirements
------------

Hardware requirements
^^^^^^^^^^^^^^^^^^^^^
- XCORE.AI EVALUATION KIT (XK-EVK-XU316)
- Camera module
- Camera ribbon connector
- 2x Micro USB cable (Power supply and xTag)
- xTag debugger and cable

Software requirements
^^^^^^^^^^^^^^^^^^^^^
- XTC tools (15.2.1): `SW_TOOLS`_
- CMake, Ninja (Windows)
- Python 3.7 or later

Building the firmware and the examples
--------------------------------------

``fwk_camera`` is intended to be used as part of an application, therefore depending if it's on its own or not, it will
be able to build examples. By following the instructions below, the user will be able to build all possible targets.

.. tab:: Linux and Mac

  .. code-block:: console

    >> Linux and Mac
    cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
    make -C build

.. tab:: Windows

  .. code-block:: console

    >> Windows
    cmake -G Ninja -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake
    ninja -C build

Building the host app (xscope_fileio)
-------------------------------------

All ``fwk_camera`` examples do ``fileio`` over a tool called ``xscope_fileio``. There is a python wrapper script ``python/run_xcore_bin.py``
which will run the example binary of the user's choice using ``xscope_fileio``. This script requires some ``xscope_fileio``
host binaries. To get host tools the user will need to follow the steps below:

1. Have a C compiler installed. If developing on Windows, it's recomended to use VS tools with a ``cl`` compiler.
2. Install the ``xscope_fileio`` package:

.. tab:: Linux and Mac

  .. code-block:: console

    >> Linux and Mac
    pip install -e utils/xscope_fileio

.. tab:: Windows

  .. code-block:: console

    >> Windows
    pip install -e utils/xscope_fileio
    cd utils/xscope_fileio/host
    cmake -G Ninja . && ninja

Running the examples
--------------------

1. Make sure the python environment is activated with installed ``xscope_fileio`` in it.
2. Run the example with the following command:

.. code-block:: console

  python python/run_xscope_bin.py <path to your example binary>
  # or
  python python/run_xscope_bin.py # and chose a binary in the dropdown menu

3. Depending on the example ran, user can get ``.bmp``, ``.raw`` or ``.bin`` files. With ``.bmp`` the image can be looked at straight 
away, whereas other formats need some more processing. Users can use one of the following python scripts to decode and see the image:

.. code-block:: console
  
  python/decode_downsampled.py
  python/decode_raw8.py
  python/decode_raw10.py
