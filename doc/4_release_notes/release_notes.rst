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
    - AE control:  auto exposure based on histogram skewness
    - AWB control: auto white balance based on a combination of grey world assumption and percentile
    - Gamma correction: 1.8 gamma correction. 
    - Image rotation: image rotation capabilities (sensor and ISP)
    - Image cropping / scaling : image scalling, cropping. 

Limitations:
*************
- RAW10 downsample: RAW10 downsample is not supported.
- Galaxy core integration it is not supported yet.

Known Issues:
*************
- Artifacts: 
    - AE: 
        Auto exposure can struggle in enviornement with high dynamic range. 
        It will choose to privilege the mayority of the image, leaving some areas underexposed or overexposed.
    - AWB: 
        Due to white balancing algorithmm, we can observe that pure color pure red or pure blue can appear more white than expected. 
        AWB can me turned off, or changed manually to adequate to a specific scene.  
