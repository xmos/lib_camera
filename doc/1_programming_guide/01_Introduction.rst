Introduction
=============

.. include:: ../substitutions.rst

Overview
---------
The purpose of this programming guide is to provide developers with a comprehensive understanding of the FWK_Camera architecture and guide them on how to effectively interact with cameras using the XCORE-AI-EXPLORER board.

The architecture consists of several key components that work together to facilitate camera communication and data processing. These components include:

1. Camera hardware and camera interface
2. Camera drivers
3. User application
4. Image signal processing
5. I/O (Using Xscope_fileIO)

Here below a high level block diagram of the FWK_Camera architecture:

.. figure:: images/1_high_level_view.png
    :alt: Alternate text for the image
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

- XMOS tools: `SW_TOOLS`_
- FWK_Camera repository: `GH_FWK_CAMERA`_
- CMake, Ninja (Windows)

Additional Resources
---------------------

- MIPI CSI-2 specification:     `MIPI`_
- XMOS I2C library user guide:  `XMOSI2C`_
- XMOS Programming Guide:       `XMOSProgrammingGuide`_
- IMX219 datasheet:             `IMX219`_
