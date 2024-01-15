Test timing
-----------

This test is used to measure the time of RAW data stream from mipi. 

Legend:

* SoF : Start of frame
* EoF : End of frame
* SoL : Start of Line
* EoL : End of Line

To run this test, from top level directory of the repo, run the following command:

.. code-block:: console
  
  cmake -G "Unix Makefiles" -S tests/hardware_tests/test_timing/ -B tests/hardware_tests/test_timing/build
  xmake -C tests/hardware_tests/test_timing/build
  xrun --xscope tests/hardware_tests/test_timing/bin/test_timing.xe
