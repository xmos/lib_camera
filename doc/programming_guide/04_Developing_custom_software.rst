Software customization
======================

.. include:: ../substitutions.rst

Adding your own sensor
----------------------

Hardware
^^^^^^^^

First you'll need to see if your sensor if compatible with XCORE.AI EVALUATION KIT (XK-EVK-XU316).
Your sensor has to:

- Support MIPI CSI2 protocol
- Be driven from 3.3V source

Software
^^^^^^^^

If you navigate to ``sesnors/api/SensorBase.hpp``, you will find the ``SensorBase`` class which is intended to be derived from.
It doesn't have anything to do with a particular sensor, it only provides API to do basic I2C communication with the sensor.
Inside ``SensorBase`` class you can also find some pulic virtual methods which will **have to** be implemented in the derived class.

In order to implement your own sensor you will need to create a directory in ``sensors/src/_your_sensor``, implement ``initialize()``,
``stream_start()``, ``stream_stop()``, ``set_exposure()``, ``configure()`` and ``control()`` methods. After that you can add your
derived class header file into ``sensors/src/sensor_control.cpp`` and have ``sensor_control()`` calling your sensor API like here:

.. code-block:: C++

  sensor_control(chanend_t c_control) {
    // first i2c_conf definition
    sensor::YOUR_SENSOR snsr(i2c_conf, other_arguments);
    snsr.control(c_control);
  }
