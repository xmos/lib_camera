|newpage|

.. _lib_camera_api_reference:

API Reference
=============

This section contains the API reference for the Camera library.

.. contents::
   :local:
   :class: this-will-duplicate-information-and-it-is-still-useful-here

Camera
------

The Camera module serves as the entry point for the camera library.

It defines global macros for main camera configuration and provides functions to configure and initialize the camera thread.

.. doxygengroup:: camera_main
   :project: lib_camera
   :members:

ISP
---

The ISP module offers an interface for configuring and controlling the camera's image processing pipeline.

It provides functions to set the output format, resolution, and various image processing parameters.
This module includes both configuration and conversion functions essential for adapting the camera output to different requirements.

.. doxygengroup:: camera_isp_cfg
   :project: lib_camera
   :members:

.. doxygengroup:: camera_isp_api
   :project: lib_camera
   :members:

.. doxygengroup:: camera_isp_conv
   :project: lib_camera
   :members:

Sensors
-------

This module provides a high-level class fort he camera sensor. 
It includes set of methods that the library needs in order to configure and control the camera sensor.

.. doxygengroup:: camera_sensors
   :project: lib_camera
   :members:

I/O
---

The I/O module includes a set of functions to read and write image data from or to a host PC. It supports various image formats. 

.. doxygengroup:: camera_io
   :project: lib_camera
   :members:
