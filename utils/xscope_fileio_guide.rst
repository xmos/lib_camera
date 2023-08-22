.. _xscope_fileio_instructions:

Installing xscope_fileio host tools
-----------------------------------

To install ``xscope_fileio``, please follow the steps below:

.. tab:: Linux and Mac

  1. Make sure you have a C compiler (e.g., gcc, clang) installed.

.. tab:: Windows

  1. Make sure you have activated VS tools environment and have ``cl`` compiler ready to use

2. Open a terminal or command prompt.

3. Install the xscope_fileio package:

.. tab:: Linux and Mac

  .. code-block:: console

    pip install -e utils/xscope_fileio

.. tab:: Windows

  .. code-block:: console

    pip install -e utils/xscope_fileio
    cd utils/xscope_fileio/host
    cmake -G Ninja . && ninja
  
Your ``xscope_fileio`` host app is now ready to use.
