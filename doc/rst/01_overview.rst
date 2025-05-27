|newpage|

.. _lib_camera_overview:

Overview
========

This section provides an overview of the library, including its purpose, supported hardware, key features, and high-level architecture.

Purpose
-------

The XMOS Camera Library is designed to provide a high-level interface for camera operations, including image capture, processing, and configuration. It abstracts the complexities of camera hardware and provides a simple API for developers to work with.

The library is intended to be connected to other XMOS libraries to perform more complex tasks, such as image classification, object tracking, and other computer vision applications. This document provides a series of examples or `Application Notes` to demonstrate how to use the library in conjunction with other XMOS libraries.

This documentation is intended for developers who either want to use the library in their applications or want to customise it to interface their own camera sensor with an XMOS device. A knowledge of camera communication protocols (MIPI, I2C) and image processing is recommended.

.. _lib_camera_supported_hardware:

Supported Hardware
------------------

Lib Camera is designed to work with the following hardware:

- **Processor**: This library is designed to work with the XMOS `xcore.ai`_ processor family (`XU316`). It is important to mention that only the packages ``265 pin FBGA`` and  ``128 pin TQFP`` include a D-PHY receiver, which is required for MIPI-CSI2 camera connection.

- **Camera Sensors**: This library is designed to work with the `IMX219`_ camera sensor. Other sensors may require modifications to the library. More information on how to add your own sensor can be found in the documentation. See :ref:`lib_camera_configuration` for more information.

- **Camera Interface**: The library uses the ``MIPI-CSI2`` interface for camera connection and I2C for camera control. The library is designed to work with the D-PHY receiver on the xcore.ai processor family. It is important to note that the library does not support the parallel camera interface (DVP) or the USB camera interface.

- **Boards**: This library works directly with the |vision board| (|vision board ref|). It can be used with other boards that have a MIPI-CSI2 interface and I2C control, such as the |explorer board| (|explorer board ref|), but some modifications may be required. More information on how to add your own board can be found in the documentation. See :ref:`lib_camera_configuration` for more information.

Key features
------------

- Image Signal Processing (ISP): The library includes ISP capabilities to enhance the image quality and transform raw image data into the requested format by the user. The included functions are demosaicing, downsampling, image scaling and cropping, image rotation, Auto Exposure (AE), and Auto White Balance (AWB).
 
- Asynchronous Capture Mode: The library supports an asynchronous still mode where the ISP thread handles camera operations in a non-blocking manner. When an external thread or application requests a frame, the ISP thread starts the camera, captures the frame, processes it, and delivers it to the application without halting its execution. This allows the application to continue performing other tasks, such as running an AI model, while the camera is capturing and processing the next frame. Subsequent frame requests can be made immediately after the first, enabling efficient and seamless integration of camera operations into the application workflow.

- Dynamic Region of Interest (ROI): The library supports dynamic ROI selection, allowing the user to select a specific region of the image for processing. This is useful for applications that require only a portion of the image to be processed, such as object detection or tracking.

- RAW8 and RGB888 data format. The library supports the following data formats: RAW8 (RGGB) and RGB888. Refer to `Camera Data Formats <https://docs.sharpcap.co.uk/2.9/10_CameraBasics.htm>`_ for more information. Data type is in ``int8_t`` by default. This can be changed via the ``demux`` options at compile time, or by converting the data using the ``camera_conversion`` functions.

- Image rotations: The library supports image rotation in two ways: 90 degrees and 180 degrees. The 90-degree rotation is performed in software, while the 180-degree rotation is performed in hardware by the camera sensor. The library also supports horizontal and vertical flipping of the image.

- Image I/O and conversion operations: The library provides a set of functions to convert the image data from one format to another or write image data to a file. The library supports only binary files or `BMP` files. The conversion functions are designed to be efficient and fast, allowing for real-time image processing. 

High-level Architecture
-----------------------

The library is structured into several key components, each responsible for a specific aspect of camera operation. The library itself is designed to only use two threads: the `MIPI Receiver` thread and the `ISP` thread. 

The high-level architecture is shown in :numref:`lib_camera-od`: 

.. _lib_camera-od:
.. figure:: ../images/lib-camera-od.drawio.svg
	:alt: High-level block diagram
	:align: center
	:width: 70%

  	High-level block diagram of the |lib_camera|.

Note that only the main components are shown in the diagram. Further details about the architecture and components are provided in the :ref:`lib_camera_architecture` section.

- `MIPI Receiver Thread`: This thread is responsible for receiving data from the camera sensor over the MIPI-CSI2 interface. It handles the low-level details of the MIPI protocol and provides data packets to the ISP thread for processing.

- `ISP Thread`: This thread is responsible for handling both MIPI receiver request for incoming data and user requests to deliver the processed data. It also handles initialisation and configuration of the camera sensor.
   
- `Sensor Control`: Encapsulates a group of functions to control the camera sensor. It handles the I2C communication with the camera sensor and provides a high-level interface for configuring the sensor settings.

- `User Thread`: This thread is not part of the library but is provided as an example of how to use the library. It is responsible for setting the buffer where the image data will be stored and for processing the image data after it has been captured. Image size and properties are user centric, meaning that the user can set the image size and properties according to their needs. The library will then handle the conversion of the image data to the desired format.

Additional Resources
--------------------

- MIPI CSI-2 specification:     `MIPI`_
- XMOS I2C library user guide:  `XMOS I2C`_
- XMOS Programming Guide:       `XMOS Programming Guide`_
- XMOS XS3A Architecture:       `XMOS XS3 Architecture`_
- Sony IMX219 datasheet:        `IMX219`_
