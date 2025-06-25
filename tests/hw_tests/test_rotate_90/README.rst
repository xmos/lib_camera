Example: Test Rotate 90
=======================

This test takes a binary image and rotates it 90 degrees. The image is then stored in a binary file. The binary file is then decoded and displayed as a PNG image.

Build and Run example
---------------------

Run the following command from the current folder: 

.. code-block:: console

    # Produce images
    python ../../../python/encode_downsampled.py --input ../imgs/orig.png --width 64 --height 64 --dtype uint8 --output ../imgs/input_rgb_uint8_64_64.bin 
    python ../../../python/encode_downsampled.py --input ../imgs/orig.png --width 64 --height 64 --dtype int8 --output ../imgs/input_rgb_int8_64_64.bin 
    # Build
    cmake -G "Ninja" -B build
    ninja -C build
    # Run 
    python ../../../python/run_xscope_bin.py bin/test_rotate_90.xe
    # Decode Image
    python decode.py
    
