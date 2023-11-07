================================
Example: Take picture downsample
================================

This example set the basic settings for the sony sensor and grab a single frame. 
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
  Make sure ``xscope_fileio`` is installed. See /utils/README.rst section for more details.

Run the following command from the example folder:

.. code-block:: console

  python ../../python/run_xscope_bin.py bin/take_picture_downsample.xe


******
Output
******

The output files ``capture.bin`` and ``capture.bmp`` will be generated at the top level the repository. ``capture.bin`` can be further processed using ``python/decode_downsampled.py`` script.
