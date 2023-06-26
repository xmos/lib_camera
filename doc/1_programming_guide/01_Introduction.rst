Introduction
=============

.. include:: ../substitutions.rst

Overview
---------
The purpose of this programming guide is to provide developers with a comprehensive understanding of the FWK_Camera architecture and guide them on how to effectively interact with cameras using the XCORE-AI-EXPLORER board.

Conventions and Terminology
---------------------------
To ensure clarity and consistency throughout this guide, the following conventions and terminology are used:

- MIPI: Mobile Industry Processor Interface. It is a standard interface specification for mobile devices, including cameras. The FWK_Camera architecture utilizes MIPI specifications for camera communication.
- Xcore: XMOS proprietary event-driven processor architecture. It provides high-performance parallel processing capabilities and is used in XMOS devices to handle camera interface and data processing.
- Channels: In the context of XMOS devices, channels are communication pathways that allow data exchange between different components. Channels play a crucial role in camera control and data transfer within the FWK_Camera architecture.
- I2C: Inter-Integrated Circuit. It is a widely used serial communication protocol for controlling and configuring devices. Within the FWK_Camera architecture, I2C is utilized for camera control operations, such as adjusting settings and retrieving sensor information.

Features
---------
- MIPI CSI2 interface
- Up to 1GBps per lane
- Low-resolution filtering
- Supported cameras:
    - IMX219
    - GC2145 (explain hardware modification)

//TODO
The |EVK_BOARD| development board features an |X|-pin MIPI CSI2 port. 
This port enables communication with cameras that are compatible with the |Xcore-AI| processor.
The processor is capable of directly processing an image from the sensor and performing various operations, 
such as converting a RAW image to an RGB image (applying ISP functions), 
analyzing the image using AI models with |xmos tools|,
converting a MIPI camera to USB interface, etc.

This repository contains a set of tools for image acquisition, processing, and transmission. 
The architecture, viewed from a high level, is composed of the following elements:

.. figure:: images/1_high_level_view.png
    :alt: Alternate text for the image
    :align: center

    High-level block diagram of the FWK_Camera.

1. Camera hardware and interface: The |tarjeta| board incorporates an MIPI connector and specific |X| hardware for transforming MIPI to |Y| ports.
2. Camera drivers: To process the image, we rely on the camera drivers, which provide a high-level API for image acquisition, filtering, and statistical analysis of the image.
3. Camera application:
4. Sensor configuration:
5. Camera output

Getting Started
----------------

Hardware requirements:

- XCORE.AI EVALUATION KIT (XK-EVK-XU316)
- Camera module
- Power supply
- Micro USB cable
- JTAG debugger

Software requirements:

- XMOS tools: `SW_TOOLS`_
- FWK_Camera repository: `GH_FWK_CAMERA`_
- CMake, Ninja (Windows)

Additional Resources
---------------------

- MIPI CSI-2 specification:     `MIPI`_
- XMOS I2C library user guide:  `XMOSI2C`_
- XMOS Programming Guide:       `XMOSProgrammingGuide`_
- IMX219 datasheet:             `IMX219`_
