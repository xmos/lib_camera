Run inject image test
---------------------

From top level directory of the repo, run the following command:

.. code-block:: console

   pytest -v -s tests/hardware_tests/inject_images/test_emulated_images.py

The images will be injected from the ``input`` folder into the device, processed and downsampled. 
The output of the pipeline will be saved in the ``output`` directory.
