Example: Test Rotate 90
=======================

This test takes a binary image and rotates it 90 degrees. The image is then stored in a binary file. The binary file is then decoded and displayed as a PNG image.

Build and Run example
---------------------

Run the following command from the current folder: 

.. code-block:: console

    # Build
    cmake -G "Ninja" -B build
    ninja -C build
    # Run 
    python ../../../python/run_xscope_bin.py bin/test_rotate_90.xe
    # Decode Image
    python decode.py
