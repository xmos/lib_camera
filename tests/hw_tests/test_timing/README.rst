Test timing
-----------

This test is used to measure the time of RAW data stream from mipi. 

Legend:

* SoF : Start of frame
* EoF : End of frame
* SoL : Start of Line
* EoL : End of Line

To run this test, from top level directory of the repo, run the following command from current directory:

.. code-block:: console

    # Build
    cmake -G "Unix Makefiles" -B build
    xmake -C build
    # Run 
    xrun --xscope --xscope-file xscope.vcd bin/test_timing.xe
    # xscope
    gtkwave xscope.vcd
