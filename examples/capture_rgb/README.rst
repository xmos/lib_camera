Example: Capture RGB
====================

This example runs intermetly a sequence of start-capture-stop. 
It prints in the console every time it recieves a:

- start of frame
- expected packet
- end of frame 

It has a delayed start, meaning that you can set up the time when you want the capture to happen.
In the future once pipeline is done, we can be able to predict image capture time and set it up accordingly.

Build and Run example
---------------------

Run the following command from the current folder: 

.. code-block:: console

    # Build
    cmake -G "Ninja" -B build
    ninja -C build
    # Run 
    xrun --xscope bin/capture_raw.xe
    # Decode Image
    python decode.py
