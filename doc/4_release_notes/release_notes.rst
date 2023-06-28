Release Notes
=============

Version 0.1.0
---------------------------

Description:
*************
This is a initital release of the fwk_camera repo. It contains a basic interface for acquiring images, process them and send them to the host. 
It also contains a basic interface for controlling the camera ISP features.

New Features:
*************

In this first release we include two basic examples to capture an image from the Sony IMX219 sensor (Raspberry Pi Camera V2).
For the first examples the image is directly captured as raw and saved as a .raw file. 
The second example is a more complex pipeline that involves the ISP. The image is captured as raw, processed by the ISP and saved as a .bmp and .bin file. 

Limitations:
*************
- RAW10 downsample: RAW10 downsample is not supported.
- Galaxy core sensor pipeline it is not yet supported.

Known Issues:
*************
- Artifacts: 
    - AE: 
        Auto exposure can struggle in enviornement with high dynamic range. 
        It will choose to privilege the mayority of the image, leaving some areas underexposed or overexposed.
    - AWB: 
        Due to automatic white balancing algorithmm ISP will try to compensante images iluminant. If the environement is a pure color pure red or pure blue, it can appear more white than expected. 
        In this case, AWB can me turned off, or changed manually to adequate to a specific scene adjusting the static AWB values.  
