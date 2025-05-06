|newpage|

Contributing
============

This section provides guidelines for contributing to the library. We welcome contributions from the community, whether it's bug fixes, new features, or improvements to the documentation.

The general approach to contributing to the library is to fork the repository, make your changes, and then submit a pull request. This allows us to review your changes and ensure they meet our quality standards before merging them into the main branch. 

When doing so, it is recommended to add tests or examples to demonstrate the new functionality or bug fix. This will help us understand the changes and ensure they work as intended.

PR's will trigger a CI pipeline that will run the tests and check the code style. Make sure new tests are added to the pipeline modifying the correct CMakeLists.txt file.

The following section focuses on contributing to the ISP components. 

Modifying or Adding an ISP component
------------------------------------

The library supports two types of ISP components: Line ISP and End-of-Frame (EOF) ISP. The primary difference between them lies in how they process image data.

Line ISP components process data on a line-by-line basis or in groups of lines. This means the component processes each line of the image as it is received from the camera sensor. This approach is typically used when converting from RAW8 input to a desired output format, such as RGB888 or YUV422. Processing line by line is preferred in this case to avoid storing the entire RAW image in memory.

End-of-Frame ISP components, on the other hand, process data after the entire image has been received and processed by the line ISP components. These components handle tasks that do not necessarily require storing the entire image, such as auto-exposure adjustments, or tasks that can be performed in-place in memory, such as int8 to uint8 conversion or gamma correction.

For modifying components, the user should be aware of the following:

- camera_isp.c : this file contain both line process under the ``handle_expected_lines()`` function and EOF process under the ``handle_post_process()`` function. 

- camera_isp.h : this file contains the different modes, and their corresponding maximum allowed regions for each mode. 

Other files that may be of interest are:

- camera.h : defines the absolute maximum sensor region. This will drive the maximum output image size.

- sensor_wrapper.h : Defines the sensor wrapper functions, which are used to communicate with the camera sensor and configure its settings. These wrapper functions are implemented in the ``sensor_wrapper.c`` file. This file serves as a C wrapper for the C++ implementation, specifically the ``sensor_imx219.cpp`` class, which is a subclass of the ``sensor_base.cpp`` class.

In order to add or modify an ISP component the user would have to submit his changes in the `isp` folder. As an example, lets suppose user want to improve the conversion from RAW8 to RGB888 (RGB1) with an improved algorithm, or wants to add a new alogirthm, like RAW8 to YUV422.

The user should implement the new algorithm in the ISP folder, ensuring it adheres to the library's coding standards and performance requirements. Additionally, the user should update the ``camera_isp.c`` file to integrate the new algorithm into the ``handle_expected_lines()`` function. 

To validate the changes, the user should ensure that the existing tests for this component are updated or extended to cover the new algorithm. Submitting the PR will trigger the CI pipeline, which will run these tests. The pipeline will verify that the new algorithm produces results comparable to the Python and OpenCV implementations by checking metrics such as `PSNR <https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio>`_ and `SSIM <https://en.wikipedia.org/wiki/Structural_similarity_index_measure>`_. These metrics ensure the output quality is acceptable when compared to the OpenCV reference implementation. For more information on ISP tests, please check the `tests/isp` folder. 
