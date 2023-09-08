Release Notes
=============

Version 0.2.0
-------------

Description
***********

In this release we've added a support for a larger resolution and reimplemented sensor control to be more flexible.

New Features
************

Added support for 1280x960 frame with the downsampling factor of 8. Sensor component has been reimplemented to be more flexible with the resolution and the RAW format.
Now it will take arguments into a class constructor rather then relying on pre-processor definitions.

Limitations
***********

- RAW10 downsample: RAW10 downsample is not supported.
- 1280x960 in RAW: 1280x960 in RAW8 ar RAW10 can't be fitted in RAM.
- Still heavily relying on the pre-processor values.

Known Issues
************

- Artefacts:

  - AE:
    Auto exposure can struggle in environment with high dynamic range.
    It will choose to privilege the majority of the image, leaving some areas underexposed or overexposed.

  - AWB:
    Due to the automatic white balancing algorithm, the ISP will try to compensate the image's illuminance. If the environment is a pure colour pure red or pure blue, it can appear more white than expected.
    In this case, AWB can be turned off, or changed manually to be adequate to a specific scene by adjusting the static AWB values.

Version 0.1.0
-------------

Description
***********
This is an initital release of the ``fwk_camera`` repo. It contains a basic interface for acquiring images, processing them and sending them to the host.
It also contains a basic interface for controlling the camera ISP features.

New Features
************

In this first release we include two basic examples to capture an image from the Sony IMX219 sensor (Raspberry Pi Camera V2).
For the first examples the image is directly captured as raw and saved as a .raw file.
The second example is a more complex pipeline that involves the ISP. The image is captured as raw, processed by the ISP and saved as a .bmp and .bin file.

Limitations
***********

- RAW10 downsample: RAW10 downsample is not supported.
- Galaxy core sensor pipeline is not yet supported.

Known Issues
************

- Artefacts:

  - AE:
    Auto exposure can struggle in environment with high dynamic range.
    It will choose to privilege the majority of the image, leaving some areas underexposed or overexposed.

  - AWB:
    Due to the automatic white balancing algorithm, the ISP will try to compensate the image's illuminance. If the environment is a pure colour pure red or pure blue, it can appear more white than expected.
    In this case, AWB can be turned off, or changed manually to be adequate to a specific scene by adjusting the static AWB values.
