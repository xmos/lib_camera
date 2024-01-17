================================
Example: Take picture downsample
================================

This example sets the basic settings for the Sony sensor and grabs a single frame. 
By default the format is the following:
- 160x120x3 RGB

*************
Build example
*************

Run the following command from the example folder: 

.. code-block:: console

  cmake -G "Unix Makefiles" -B build
  xmake -C build


***************
Running example
***************

.. warning::
  Make sure ``xscope_fileio`` is installed (folow top level README.rst for installation)

Run the following command:

.. code-block:: console

  python ../../python/run_xscope_bin.py bin/take_picture_downsample.xe # from the example folder
  python python/run_xscope_bin.py examples/take_picture_downsample/bin/take_picture_downsample.xe # from the top level folder

******
Output
******

The output files ``capture.bin`` and ``capture.bmp`` will be generated at the top level the repository. 
``capture.bin`` can be further processed using ``python/decode_downsampled.py`` script.
