Introduction
=============

.. include:: ../substitutions.rst

Overview
---------
The purpose of this programming guide is to provide developers with a comprehensive understanding 
of the ``fwk_camera`` architecture and guide them on how to effectively interact with cameras using the XCORE-AI-EXPLORER board.

Conventions and Terminology
---------------------------
To ensure clarity and consistency throughout this guide, the following conventions and terminology are used:

- MIPI: Mobile Industry Processor Interface. It is a standard interface specification for mobile devices, including cameras. The ``fwk_camera`` architecture utilizes MIPI specifications for camera communication.
- Xcore: XMOS proprietary event-driven processor architecture. It provides high-performance parallel processing capabilities and is used in XMOS devices to handle camera interface and data processing.
- Channels: In the context of XMOS devices, channels are communication pathways that allow data exchange between different components. Channels play a crucial role in camera control and data transfer within the ``fwk_camera`` architecture.
- I2C: Inter-Integrated Circuit. It is a widely used serial communication protocol for controlling and configuring devices. Within the ``fwk_camera`` architecture, I2C is utilized for camera control operations, such as adjusting settings and retrieving sensor information.

Features
---------
The XCORE-AI-EXPLORER board features an 15-pin MIPI CSI2 port (compatible with Raspberry PI). 
This port is connected to the Xcore-AI processor, so we can directly processing an image from an external sensor and performing various operations, 
such as converting a RAW image to an RGB image (applying ISP functions), 
analyzing the image using AI models with xmos-ai-tools,
converting a MIPI camera to other interfaces as USB, SPI, etc.

The FWK_Camera alongside with the Explorer board architecture provides the following features:

- MIPI CSI2 interface
- Up to 1GBps per lane
- Low-resolution filtering
- Supported cameras:
  - IMX219

This repository contains a set of tools for image acquisition, processing, and transmission. 
The architecture, viewed from a high level, is composed of the following elements:

.. figure:: images/1_high_level_view.png
  :alt: High-level block diagram
  :align: center

  High-level block diagram of the ``fwk_camera``.

1. Camera hardware and interface
2. Camera drivers
3. Camera application
4. Sensor configuration
5. Camera output

Each of these elements is described in detail in the following sections.

Getting Started
----------------

To start using the ``fwk_camera``, you can proceed to the Quick Start Guide, go to: 

:ref:`QS_FWKC`.

Additional Resources
---------------------

- MIPI CSI-2 specification:     `MIPI`_
- XMOS I2C library user guide:  `XMOSI2C`_
- XMOS Programming Guide:       `XMOSProgrammingGuide`_
- IMX219 datasheet:             `IMX219`_
