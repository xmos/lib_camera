Test: isp
=========

This test takes sevral raw images and perofrms demosaic in each one. 
It compares the output in Python and C implementation. 

If metrics are above trehshold, test fails. 
Metrics can be checked in ``utils.py``.

Build and Run example
---------------------

Use the current folder for all commands.

To build on Windows:

.. code-block:: console

    cmake -G Ninja -B build
    ninja -C build

To build on Linux:

.. code-block:: console

    cmake -G "Unix Makefiles" -B build
    xmake -C build

To run the test:

.. code-block:: console

    python test_rgb.py
    
To run only the C version manually:

.. code-block:: console

    xsim --xscope "-offline trace.xmt" bin/test_isp_rgb1.xe
    # or xrun 
    xrun --xscope bin/test_isp_rgb1.xe
