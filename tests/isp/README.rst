Test: isp
=========

This test takes sevral raw images and perofrms demosaic in each one. 
It compares the output in Python and C implementation. 

If metrics are above trehshold, test fails. 
Metrics can be checked in ``utils.py``.

Build and Run example
---------------------

Run the following command from the current folder: 

.. code-block:: console

    python test_isp.py
    
To compile and run only the C version manually:

.. code-block:: console

    cmake -G Ninja -B build
    ninja -C build
    xsim --xscope "-offline trace.xmt" bin/test_isp_rgb1.xe
    # or xrun 
    xrun --xscope bin/test_isp_rgb1.xe
