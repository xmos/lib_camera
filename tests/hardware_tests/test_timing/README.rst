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

  xrun --xscope build/tests/hardware_tests/test_timing/test_timing.xe
