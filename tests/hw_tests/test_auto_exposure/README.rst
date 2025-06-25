Test: Auto Exposure
===================

This test is designed to verify the functionality of the auto exposure feature in the camera module. 
It checks if the camera can automatically adjust its exposure settings based on the lighting conditions in the environment.

This example buils the same test with different cmake options.

- AWB and AE disabled
- AWB enabled and AE disabled
- AWB and AE enabled

Then a python script is used to run the test and visualize the results.

Build and Run
-------------

Run the following command from the current folder: 

.. code-block:: console

    # Build
    cmake -G "Unix Makefiles" -B build
    xmake -C build
    # Run 
    python run.py
