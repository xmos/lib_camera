=========================
Example: Take picture RAW
=========================

This example set the basic settings for the sony sensor and grab a single frame. 
By default the format is the following:
- 640x480 RAW8

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
    
    python python/run_xscope_bin.py examples/take_picture_raw/bin/take_picture_raw.xe


******
Output
******

The output file ``capture.bin`` will be generated at the top level the repository. It can be further processed using ``python/decode_raw8.py`` script.
