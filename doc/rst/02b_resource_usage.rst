.. _lib_camera_resource_usage:

Resource Usage
==============

This section describes the resources required by ``lib_camera`` during typical operation.

**Quick Summary**
-----------------

For a typical single capture RGB application, ``lib_camera`` requires:

* **Hardware**: 3x 1-bit ports + 1x 8-bit port + 1 clock block (MIPI interface)
* **Software**: 2 threads (MIPI receiver + image processing)
* **Memory**: ~170 KB total (50 KB library + 120 KB image buffer)
* **Performance**: 10-30 fps typical, single-capture optimized

The following describes a more detailed breakdown of the resources used by the library.

Ports, Pins, and Clocks
-----------------------

The xcore.ai series of devices have an integrated MIPI transceiver. It uses several ports and a clock block to interface with the camera sensor.

The MIPI D-PHY is a receiver-only physical layer, accepting up to two differential lanes at data rates up to 1.5 GHz. A dedicated demultiplexer hardware multiplexes the two lanes and can optionally unpack common image formats.

:numref:`resource-usage-table` shows the ports and clock blocks used by the MIPI D-PHY interface.

.. list-table:: MIPI Communication Ports and Clock Blocks
    :header-rows: 1
    :name: resource-usage-table

    * - Resource
      - Description
    * - ``Port 1O``
      - MIPI Clock. 1-bit port used for clocking the demultiplexer and the data port at the same frequency via a clock block. Configured via tile registers. Typically set to 100-200 MHz.
    * - ``Port 8A``
      - MIPI Data. 8-bit port for receiving MIPI data from the demultiplexer. Clocked by the MIPI clock.
    * - ``Port 1I``
      - MIPI Valid. 1-bit port indicating when data on the data port is valid.
    * - ``Port 1E``
      - MIPI Active. 1-bit port indicating when data on the data port is active.
    * - ``MIPI_CLKBLK``
      - MIPI clock block. Generates the MIPI clock from a reference clock.

Thread Usage
------------

The library uses **two threads** to handle different tasks:

- 1x MIPI Receiver Thread: Handles the low-level MIPI protocol, including lane synchronization, data decoding, and error handling. 
- 1x Image Processing Thread: Processes the raw image data, including debayering, color correction, and image scaling. It also manages sensor configuration via *I2C*.

.. note::

   The library is designed to be flexible and can be adapted to different hardware configurations. Depending on the specific use case, additional threads may be required for tasks such as image analysis or storage. For instance, if *I2C* port is placed in a different tile, an additional thread is required to handle the *I2C* communication.

Memory
------

The memory requirements depend on the library itself and user image memory. Image memory depends on the resolution and format required by the application. Below more details of memory usage are provided.

- **Library Memory**: The library itself requires approximately 50 KB of memory for code and data structures. This includes the MIPI protocol handling, image processing algorithms, and sensor configuration management.
- **Image Memory**: The memory required for image buffers depends on the resolution and format of the images being captured. For example:

  - A 640x480 RAW8 image requires approximately 307.2 KB of memory (640 * 480 * 1 bytes).
  - A 200x200 RGB image requires approximately 120 KB of memory (200 * 200 * 3 bytes).

For a typical application capturing RGB888 at 200x200 resolution, the total memory requirement would be approximately **170 KB** (50 KB for the library + 120 KB for the image buffer).

Frame Rates
-----------

Expected frame rates are typically between ``10fps`` and ``30fps`` with default configurations.

The PHY supports a maximum of 1.5 Gbps per lane, but the effective bandwidth depends on the sensor capabilities, port bandwidth, demultiplexing operations, and other factors. The demultiplexed data rate is generally no more than 150 MBps. A 100 MHz thread can achieve up to 200 MBps. The shim clock should be set to a maximum of 200 MHz; higher clock rates may cause issues like empty pixels or receiver errors. As a general guideline, assume a maximum of 0.8 Gbps per lane for reliable operation.

This library is designed for single-capture predominant mode. It captures a single image and then stops, allowing other tasks to run between captures, such as model inference or transmitting the image over a network. This approach is not optimized for continuous streaming. While high frame rates can be achieved from Start of Frame (SOF) to End of Frame (EOF), the interval between EOF and the next SOF is not optimized.

For the IMX219 sensor, framerate and resolution are linked. An external frequency, known as the input clock (INCK), along with a set of dividers and PLLs, is used to configure both the CMOS side (domain 1) and the FIFO/MIPI clock output (domain 2). The following explains the key parameters involved:

- **Sensor size**: The maximum resolution the sensor can capture (3280x2464).  
- **Active region**: The area of the sensor currently used for capturing images. The difference corresponds to the blanking time.
- **Data rate**: This is the amount of data the sensor needs to output per second.
- **Pixel clock (pxl_clk)**: This is the clock used to readout the pixels from the sensor. Typically half of the data rate for this sensor.  
- **Input clock**: The external clock supplied to the sensor, usually 24 MHz.  
- **Vertical Timing Domain**: Controls the timing for pixel readout from the sensor's active region. Managed by the ``VT_`` registers.
- **Output Clock Domain**: Governs the timing for data transmission over the MIPI interface. The output clock domain sets the speed at which image data is serialized and sent to the host. Managed by the ``OP_`` registers.

Configuration Example
---------------------

For example, to achieve 200x200x3 RGB image at 30 fps in RGB4 mode, the following parameters are used:

- Active region: 800 (height) x 800 (width). 
- Sensor region: 1232 (height) x 3280 (width), to allow line and frame blanking.
- Data rate: 1232 * 3280 * 30 = 121.3 MBps.
- Pixel clock: 121.3 / 2 = 60.65 MHz.
- Input clock: 24 MHz.
- Output clock: 100-150 MHz.
- Vertical Timing Pre-Divider1 and Divider1: PREPLLCK_VT_DIV (0x02) and VTPXCK_DIV (0x08). 
- Vertical Timing PLL Multiplier1: 60.65 * 8 / (24 / 2) = 40. PLL_VT_MPY (0x28). 
- Output Timing Pre-Divider2 and Divider2: PREPLLCK_OP_DIV (0x02) and OPPXCK_DIV (0x08).
- Output Timing PLL Multiplier2: 100 * 8 / (24 / 2) = 66.67 (0x42). PLL_OP_MPY (0x42).

For updating those parameters check the ``sensor_imx219_reg.h`` file in the ``lib_camera/src/sensors/`` folder.
This will give a frame time from SOF to EOF of 33.3 ms. 
