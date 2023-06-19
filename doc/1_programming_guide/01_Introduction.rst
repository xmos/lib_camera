Introduction
=============

.. include:: images/image_names.rst
.. contents:: Table of Contents

Overview
---------
The purpose of this programming guide is to provide developers with a comprehensive understanding of the FWK_Camera architecture and guide them on how to effectively interact with cameras using XMOS devices.

The architecture consists of several key components that work together to facilitate camera communication and data processing. These components include:

- Camera hardware and camera interface
- Camera drivers
- User application / user interface
- Image signal processing
- I/O

    .. figure:: images/{figure1}
        :alt: High Level Block Diagram
        :figwidth: 400px
        High Level Block Diagram

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

- XMOS device
- Camera module
- Power supply
- USB cable
- JTAG debugger

Software requirements:

- XMOS tools: https://www.xmos.ai/software-tools/
- FWK_Camera repository
- CMake, Ninja (Windows)

Additional Resources
---------------------
.. _MIPI: https://www.mipi.org/specifications/csi-2
.. _XMOS: https://www.xmos.ai/
.. _XMOSI2C: https://www.xmos.ai/download/lib_i2c-%5Buserguide%5D(5.0.0rc3).pdf
.. _XMOSProgrammingGuide: https://www.xmos.ai/download/XMOS-Programming-Guide-(documentation)(E).pdf
.. _IMX219: https://www.opensourceinstruments.com/Electronics/Data/IMX219PQ.pdf

- MIPI CSI-2 specification:     `MIPI`_
- XMOS I2C library user guide:  `XMOSI2C`_
- XMOS Programming Guide:       `XMOSProgrammingGuide`_
- IMX219 datasheet:             `IMX219`_
