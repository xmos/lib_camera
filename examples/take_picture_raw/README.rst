Example: take picture RAW
=========================

This example set the basic settings for the sony sensor and grab a single frame. 
By default the format is the following:
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

  python ../../python/run_xscope_bin.py bin/take_picture_raw.xe # from the example folder


Output
------

The output files ``capture.bin`` and ``capture.bmp`` will be generated at the top level the repository. 
``capture.bin`` can be further processed using ``python/decode_downsampled.py`` script.
