Camera Library
==============

This repository serves as a comprehensive software solution for camera manipulation using the XCORE.AI sensor.

Repository Structure
--------------------

- **doc**         : documentation for the camera library
- **examples**    : examples for taking pictures with the ``XCORE-VISION board``
- **lib_camera**  : library to manipulate cameras
- **python**      : python functions to decode RAW8, RAW10 pictures and other utilities to treat images
- **tests**       : tests for the camera library

Examples
--------

- **take_picture_downsample** : takes a picture from an input size of 680x480 or 1280x960 and downsample it down to 160x120. 


How to Use
----------

.. code-block:: c

    // ---- main.xc
    #include <camera.h>
    ...
    chan c_cam[2];
    on tile[1]: camera_main(c_cam);
    ...


    // Define image configuration
    camera_configure_t config = {
        .offset_x = 0.2,
        .offset_y = 0.1,
        .sx = 1, // Note: in raw sx sy has to be 1
        .sy = 1,
        .shx = 0.0,
        .shy = 0.0,
        .angle = 0.0,
        .T = NULL,
    };
    
    // Create an Image Structure
    int8_t image_buffer[H][W][CH] = {{{0}}};
    int8_t* image_ptr = &image_buffer[0][0][0];
    image_cfg_t image = {
        .height = H,
        .width = W,
        .channels = CH,
        .ptr = image_ptr,
        .config = &config
    };

    // Send the image configuration to the ISP
    camera_isp_coordinates_compute(&image);
    chan_out_buf_byte(c_user_to_isp, (uint8_t*)&image, sizeof(image_cfg_t));

    // Get the image from the ISP
    chan_in_byte(c_isp_to_user); // wait for the image
    

Quick Start
-----------

In order to setup the project and build the examples, please refer to the documentation:

`doc/quick_start_guide <./doc/quick_start_guide/quick_start_guide.rst>`_.
