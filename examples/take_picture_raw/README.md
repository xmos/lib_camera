## Example: Take picture

This example set the basic settings for the sony sensor and grab a single frame. 
By default the format is the following:
* 640x480 RAW8

### File Description
* Sensor.h : configures and set the basic settings for the sensor
* Main.xc  : starts 2 threads, i2C control and mipi capture
* Process_frame.c : write the image to a file 

## Build example
Run the following command: ```make example_take_picture``` 