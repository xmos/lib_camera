Example: take picture downsample
================================

This example sets the basic settings for the Sony sensor and grabs a single frame. 
By default, the output format is the following:
- 160x120x3 RGB


Build example
-------------
Run the following command from the example folder: 

.. code-block:: console

  cmake -G "Unix Makefiles" -B build
  xmake -C build


Running example
---------------

.. warning::

  Make sure ``xscope_fileio`` is installed (follow `doc/quick_start_guide <../../doc/quick_start_guide/index.rst>`_. for installation)

Run the following command from the current directory:

.. code-block:: console

  python ../../python/run_xscope_bin.py bin/take_picture_downsample.xe # from the example folder


Output
------

The output files ``capture.bin`` and ``capture.bmp`` will be generated at the top level the repository. 
``capture.bin`` can be further processed using ``decode_downsampled.py`` script inside the top level ``python`` folder.
