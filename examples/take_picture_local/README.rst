Example: take picture local
===========================

This example set the basic settings for inject a RAW image
to the explorer board an run the ISP pipeline. 

If you do not have a RAW8 image, you can produce it with 
the utility encode_raw8 from the python folder.

The image default name is  ``temp.raw ``

By default the input format is the following:
- 640x480 RAW8


Build example
-------------
Run the following command from the example folder: 

.. code-block:: console

  cmake -G "Unix Makefiles" -B build
  xmake -C build


Running example
---------------

.. warning::

  Make sure ``xscope_fileio`` is installed (follow `doc/quick_start_guide <./doc/quick_start_guide/index.rst>`_. for installation)

Run the following command from the current directory:

.. code-block:: console

  python ../../python/run_xscope_bin.py bin/take_picture_local.xe # from the example folder


Output
------

The output files ``capture.bin`` and ``capture.bmp`` will be generated at the top level the repository. 
``capture.bin`` can be further processed using ``python/decode_downsampled.py`` script.
