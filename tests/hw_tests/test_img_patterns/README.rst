test_img_patterns
=================

This test sets the test pater modes of the camera one by one and captures each one of them to a file. 
Then a python script decodes them to png images and plots them in a figure. 

Build and Run
-------------

Run the following command from the current folder: 

.. code-block:: console

    # Build
    cmake -G "Ninja" -B build
    ninja -C build
    # Run 
    xrun --xscope bin/test_img_patterns.xe
    # Decode Image
    python decode.py
