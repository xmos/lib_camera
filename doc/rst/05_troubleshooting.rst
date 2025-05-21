|newpage|

.. _lib_camera_troubleshooting:

Troubleshooting
===============

Common Issues
-------------

This section provides solutions to common issues that users may encounter when using the library.

#. **Camera Not Detected**: If the camera is not connected or not properly connected, the system will fail to detect it. In these cases, the board will blink red and the following error message will be displayed:

.. code-block:: console

    TODO

To resolve this issue, ensure that the camera is properly connected to the board. If the camera is not detected, check the connection and try again.

#. **Invalid Image Configuration**: If the image configuration is invalid, the system will not be able to process the image correctly. This can happen if the image size is not supported by the camera or if the image format is not supported. In this case, the following error message will be displayed:

.. code-block:: console

    xrun: Program received signal ET_ECALL, Application exception.
    0x00081a9a in camera_isp_coordinates_compute (img_cfg=0xd2280)

To resolve this issue, ensure that the image configuration is valid and supported by the camera. Check the camera documentation for supported image sizes and formats.

#. **Missing Timings**: It may occur that the camera is not able to meet the timings for a specific mode. This can be due to a variety of reasons, such as the camera running out of time while processing the current line before the next one arrives, the camera is configured faster than it can handle, either the PHY or the SHIM. If this happens, the camera will not be able to meet the required timing and the ISP will receive invalid packets. This will result in an error message similar to the following:

.. code-block:: console

    xrun: Program received signal ET_ECALL, Application exception.
    0x00081c6a in handle_unknown_packet (data_type=<optimized out>) 
    at _camera_isp.c:77

#. **Incorrect Capture Sequence**: This issue may occur if ISP functions are called in the wrong order or too early. For example, calling ``camera_isp_start_capture()`` before computing the coordinates with ``camera_isp_coordinates_compute()`` can result in undefined behaviour, often leading to image corruption. Conversely, if ``camera_isp_get_capture()`` is called before ``camera_isp_start_capture()``, the ISP will block while waiting for the image to be captured indefinitely. 
   
#. **Incorrect Tile Placement**: Ensure that both the caller thread and the `camera_main` thread are running on the same tile (by default, tile 1). If they are placed on different tiles, code attempts to jump, branch, or return to an address that is not part of executable memory.

.. code-block:: console

    Frame time: 4805210 cycles
    xrun: Program received signal ET_ILLEGAL_PC, Illegal program counter.
    0x4c086f7e in ?? ()

#. **Image Corruption**: Image corruption can occur if the image buffer is modified before the image has been fully captured. If the image is modified while the ISP is processing it, or if the pointer is used during capture, this may result in undefined behavior and data corruption. To prevent this issue, ensure that the image is not modified or accessed until the ISP has completed processing.
