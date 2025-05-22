|newpage|

.. _lib_camera_architecture:

Architecture
============

This section provides a detailed overview of the architecture of the library, including its components, data flow, and concurrency model. It is intended for developers who want to understand how the library works and how to customise it for their own applications.

Components
----------

In this library, components refer to a high-level description of the main parts of the library. The library is composed of several components, each responsible for a specific task. A component could be a group of modules, a class, a function or a thread. The main components of the library are:

.. list-table:: lib_camera components
    :header-rows: 1
    :widths: 25 75

    * - Component
      - Description
    * - Sensor
      - The sensor refers to the camera hardware and its drivers, located in the ``src/sensors`` directory. Each sensor is implemented as a class that inherits from a generic base class (``sensor_base.cpp``). For example, ``sensor_imx219.cpp`` provides support for the IMX219 sensor. A C++ wrapper (``sensor_wrapper.cpp``) allows these classes to be used from C code, as called by ``cameras_isp``.
    * - Sensor Control
      - Sensor control covers all I2C operations for configuring the sensor, such as reading or writing registers for pixel format, exposure time, data format, and clock configuration.
    * - Sensor Region
      - The Sensor Region, defined in ``camera.h``, is a macro specifying the maximum image size the sensor can capture. This affects the achievable frame rate and selectable region. The maximum region is set by the sensor's active pixel area (e.g., 3280x2464 for IMX219). Downsampling and binning reduce this size. The ISP will reject regions larger than allowed, but the maximum can be adjusted based on user needs and sensor capabilities.
    * - MIPI D-PHY SHIM
      - The MIPI Software Interface Module (SHIM) consist of a MIPI D-PHY receiver and a demultiplexer, which translates MIPI Lanes data into xcore ports. Usually, the MIPI Shims receive two MIPI data lanes, which are translated into 4 xcore ports, active, valid, clock and data.
    * - MIPI Receiver
      - The MIPI receiver thread is a software thread that takes as input the mentioned 4 points and retrieves useful MIPI packets, those MIPI packets, such as header and data packets. It is located in ``src/mipi``. 
    * - ISP
      - In this context, the ISP englobes all the image processing functions, from MIPI packets to a desired output image. It consists of line-by-line processes as well as after the end of frame (EOF) functions. They are located in ``src/isp``.
    * - User Thread 
      - The user thread or consumer is the thread or application that specifies the image and consumes it. This thread needs to call functions from the ISP that will allow configuring the desired image and taking a picture. This code is also located in ``src/isp``. This library provides examples of how the user thread or consumer would look.

Image Processing Pipeline
-------------------------

The image processing pipeline consists of several stages, including demosaicing, downsampling, image scaling and cropping, image rotation, Auto Exposure (AE), and Auto White Balance (AWB). Each stage is implemented as a separate function in the library, allowing for easy customisation and extension.
:numref:`diagram-isp-pipeline` illustrates the image processing pipeline:

.. _diagram-isp-pipeline:
.. uml:: ../images/diagram-isp-pipeline.uml
    :alt: ISP Pipeline Diagram
    :caption: ISP Pipeline Diagram
    :align: center
    :height: 320px

This diagram illustrates a basic image processing pipeline. It starts with RAW8 RGGB input data, followed by demosaicing and downsampling. Histograms and statistics are computed to support auto-exposure (AE) and auto white balance (AWB). Optional operations include cropping, resizing, rotating, and data type conversion. The final result is an output image.

Capture Sequence Diagram
------------------------

:numref:`diagram-isp-sequence` illustrates the interaction between the main components of the library during the image capture process. 
It shows how the MIPI receiver thread and the ISP thread work together to capture and process an image.

This diagram outlines the sequence of operations in the camera capture pipeline. The main thread starts the ISP and MIPI threads. The user thread prepares the image buffer, image metadata (`image_t`), and configuration (`image_cfg_t`). The ISP thread initialises the camera sensor via I2C. The user then computes ISP coordinates and begins capture for each frame.

For every frame, the sensor is started, and MIPI sends line packets to the ISP. Based on the packet type (`FRAME_START`, `RAW8`, or `FRAME_END`), the ISP updates counters, processes expected lines, or performs post-processing (statistics, auto-exposure, auto white balance). Once the frame is complete, the sensor is stopped, and the captured image is retrieved.

.. _diagram-isp-sequence:
.. uml:: ../images/diagram-isp-sequence.uml
    :alt: Capture Sequence Diagram
    :caption: Capture Sequence Diagram
    :align: center
    :width: 90%
