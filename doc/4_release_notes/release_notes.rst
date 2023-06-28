Release Notes
=============

Version 0.0.1
---------------------------

Description:
*************
This is a initital release of the fwk_camera repo. It contains a basic interface for adquiring images, process them and send them to the host. 
It also contains a basic interface for controlling the camera ISP features.

New Features:
*************

- Raw8 capture: Capture a raw-8 image from the camera and send it to the host.
- Raw8 capture and downsample: capture a raw image from the camera, downsample it by 4 and send it to the host.
- Raw10 mode: mode to capture raw10 images from the camera.
- ISP features:
    - AE control
    - AWB control
    - Gamma correction
    - Image rotation
    - Image cropping / scaling


Known Issues:
*************

- RAW10 downsample: 
- Artifacts: 
- Camera binning modes: 
