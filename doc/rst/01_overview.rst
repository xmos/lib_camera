Overview
========

This section provides an overview of the library, including its purpose, supported hardware, key features, and high-level architecture.

Purpose
-------

The XMOS Camera Library is designed to provide a high-level interface for camera operations, including image capture, processing, and configuration. It abstracts the complexities of camera hardware and provides a simple API for developers to work with.

The library is intended to be connected to other XMOS libraries to perform more complex tasks, such as image classification, object tracking, and other computer vision applications. A series of examples or ``Application Notes`` are provided further this document to demonstrate how to use the library in conjunction with other XMOS libraries.

This documentation is intended for developers who either want to use the library in their applications or want to customize the library to interface their own camera sensor with an XMOS device. A knowledge of camera communication protocols (MIPI, I2C) and image processing is recommended.

Supported Hardware
------------------

Lib Camera is designed to work with the following hardware:

- **Processor**: This library is designed to work with the XMOS `xcore.ai`_ processor family. It is important to mention that only the packages ``265 pin FBGA`` and  ``128 pin TQFP`` include a D-PHY receiver, which is required for MIPI-CSI2 camera connection.

- **Camera Sensors**: This library is designed to work with the `IMX219`_ camera sensor. Other sensors may require modifications to the library. More information on how to add your own sensor can be found in the documentation. See :ref:`lib_camera_configuration` for more information.

- **Camera Interface**: The library uses the ``MIPI-CSI2`` interface for camera connection and I2C for camera control. The library is designed to work with the D-PHY receiver on the xcore.ai processor family. It is important to note that the library does not support the parallel camera interface (DVP) or the USB camera interface.

- **Boards**: This library works directly with the |vision board| (|vision board ref|). It can be used with other boards that have a MIPI-CSI2 interface and I2C control, such as the |explorer board| (|explorer board ref|), but some modifications may be required. More information on how to add your own board can be found in the documentation. See :ref:`lib_camera_configuration` for more information.

Key features
------------

- On the fly Image Signal Processing (ISP): 
- Asynchronous streaming mode.
- RAW8 and RGB888 image capture.
- Dynamic Region of Interest (ROI) selection.
- Support for multiple downsample factors: x1, x2, x4.
- Image rotations.
- Image I/O and conversion operations. 

High-level Architecture
-----------------------

The library is structured into several key components, each responsible for a specific aspect of camera operation. The high-level architecture is shown in the following diagram:

.. figure:: ../images/lib-camera-od.drawio.svg
	:alt: High-level block diagram
	:align: center
	:width: 70%

  	High-level block diagram of the |lib_camera|.

Note that only the main components are shown in the diagram. Further details on the architecture is provided in the :ref:`lib_camera_architecture` section.

- MIPI Receiver Thread: This thread is responsible for receiving data from the camera sensor over the MIPI-CSI2 interface. It handles the low-level details of the MIPI protocol and provides data packets to the ISP thread for processing.

- ISP Thread: This thread is responsible for handling both MIPI Reciever request for incoming data and user requests to deliver the processed data. It also handles initialization and configuration of the camera sensor.
   
- Sensor Control: Encapsulates a group of functions to control the camera sensor. It handles the I2C communication with the camera sensor and provides a high-level interface for configuring the sensor settings.

- User Thread: This thread is not part of the library but is provided as an example of how to use the library. It is responsible for setting the buffer where the image data will be stored and for processing the image data after it has been captured. Image size and properties are user centric, meaning that the user can set the image size and properties according to their needs. The library will then handle the conversion of the image data to the desired format.

Additional Resources
--------------------

- MIPI CSI-2 specification:     `MIPI`_
- XMOS I2C library user guide:  `XMOS I2C`_
- XMOS Programming Guide:       `XMOS Programming Guide`_
- XMOS XS3A Architecture:       `XMOS XS3 Architecture`_
- Sony IMX219 datasheet:        `IMX219`_
