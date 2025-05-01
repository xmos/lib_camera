:orphan:

##########################
lib_camera: Camera Library
##########################

:vendor: XMOS
:version: 2.0.0
:scope: General Use
:description: Camera Library
:category: Vision
:keywords: MIPI, Camera, I2C, RAW, ISP
:devices: xcore.ai

*******
Summary
*******

``lib_camera`` is a library for controlling cameras using XCORE.AI It provides an API for manipulating cameras, and perofrming Image Singal Processing (ISP) functions.

********
Features
********

- Camera Interface:
  - MIPI-CSI2 interface for camera connection
  - I2C interface for camera control

- Sensor Supported:
  - Sony IMX219 (Raspberry Pi Camera V2)

- Capture and ISP:
  - RAW8 capture.
  - RGB888 capture. 
  - Asynchronous Streaming Mode.
  - Dynamic Region of interest (ROI) selection.
  - Support the following downsample factors: x1, x2, x4. 
  - Auto White Balance (AWB) control.
  - Auto Exposure (AE) control.
  - Sw Image rotation (90 degrees).
  - Sensor Image rotation (180 degrees).
  - Sensor Image Flip (horizontal and vertical).

************
Known issues
************

- The library is designed to work with the IMX219 sensor. Other sensors may require  modifications to the library. More information on how toadd your own sensor can be found in the documentation.
 
- White balancing is based on static white balancing of the sensor IMX219. 
The static values are not optimal for all lighting conditions. When using a different sensor, the static values should be adjusted to match the sensor's characteristics.

- Output Image Size must be a multiple of four. 

****************
Development repo
****************

  * `lib_camera <https://www.github.com/xmos/lib_camera>`_

**************
Required tools
**************

  * XMOS XTC Tools: 15.3.1

*********************************
Required libraries (dependencies)
*********************************

  * `lib_i2c <https://www.xmos.com/file/fwk_io>`_
  * `lib_logging <https://www.xmos.com/file/lib_logging>`_

*************************
Related application notes
*************************

The following application notes use this library:

  * `AN02017` <www.xmos.com/file/an02017>`_
  * `AN02005` <www.xmos.com/file/an02005>`_
  * `AN02010` <www.xmos.com/file/an02010>`_
  * `AN02013` <www.xmos.com/file/an02013>`_

*******
Support
*******

This package is supported by XMOS Ltd. Issues can be raised against the software at
`http://www.xmos.com/support <http://www.xmos.com/support>`_
