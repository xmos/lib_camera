Software customisation
======================

Adding a new sensor
-------------------

This section describes how to add a new sensor to the xcore.ai camera library.

Hardware
^^^^^^^^

First the user will need to see if their sensor is compatible with the |vision board|.

The sensor has to:

- Support MIPI CSI2 protocol
- Be driven from 3.3V source
- Have a compatible FPC-24 camera connector.

The following diagram shows the pinout of the 24-pin FPC connector on the |vision board|:

.. figure:: images/vision_board_fpc.png
  :align: center
  :alt: FPC-24 connector pinout
  
  FPC-24 connector pinout

Software
^^^^^^^^

By navigating to ``sensors/api/SensorBase.hpp``, the user will find the ``SensorBase`` class which is intended to be derived from.
It doesn't have anything to do with a particular sensor, it only provides an API to do basic I2C communication with the sensor.
Inside ``SensorBase`` class users can also find some public virtual methods which will **have to** be implemented in the derived class.

In order to implement a new sensor the user will need to create a directory in ``lib_camera/src/sensors``, implement a derived class with 
``initialize()``, ``stream_start()``, ``stream_stop()``, ``set_exposure()``, ``configure()`` and ``control()`` methods. When the
sensor class has been implemented, its header file can be added into ``lib_camera/src/sensor_control.cpp``. ``sensor_control()`` should be
calling the new sensor API like this:

.. code-block:: C++

  sensor_control(chanend_t c_control) {
    // i2c_conf definition first
    sensor::NEW_SENSOR snsr(i2c_conf, other_arguments);
    snsr.control(c_control);
  }

After that's been done, the user will need rebuild the application. 
