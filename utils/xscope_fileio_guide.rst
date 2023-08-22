.. _xscope_fileio_instructions:

Installing xscope_fileio
------------------------

To install `xscope_fileio`, please follow the steps below:

1. Make sure you have a C/C++ compiler (e.g., cl compiler) installed.

2. Open a terminal or command prompt.

3. Install the xscope_fileio package:

Linux, Mac
~~~~~~~~~~

.. code-block:: console
      
      pip install -e utils/xscope_fileio

Windows
~~~~~~~

.. code-block:: console

      pip install -e utils/xscope_fileio
      cd utils/xscope_fileio/host
      cmake -G Ninja . && ninja
      
Your xscope_fileio host app is now ready to use.
