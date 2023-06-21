Introduction
=============

.. include:: ../substitutions.rst
.. contents:: Table of Contents

Overview
---------
The purpose of this programming guide is to provide developers with a comprehensive understanding of the FWK_Camera architecture and guide them on how to effectively interact with cameras using the XCORE-AI-EXPLORER board.

The architecture consists of several key components that work together to facilitate camera communication and data processing. These components include:

- Camera hardware and camera interface
- Camera drivers
- User application / user interface
- Image signal processing
- I/O

.. figure:: images/1_high_level_view.png
    :alt: Alternate text for the image
    :width: 400px
    :align: center

    High level block diagram of fwk camera. 



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
- Low resolution filtering
- Cameras supported:
    - IMX219
    - GC2145 (explain hardware modification)

Getting Started
----------------
Hardware requirements:

- XCORE.AI EVALUATION KIT (XK-EVK-XU316)
- Camera module
- Power supply
- Micro USB cable
- JTAG debugger

Software requirements:

- XMOS tools: https://www.xmos.ai/software-tools/
- FWK_Camera repository
- CMake, Ninja (Windows)

Additional Resources
---------------------

- MIPI CSI-2 specification:     `MIPI`_
- XMOS I2C library user guide:  `XMOSI2C`_
- XMOS Programming Guide:       `XMOSProgrammingGuide`_
- IMX219 datasheet:             `IMX219`_
