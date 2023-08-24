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

In order to implement your own sensor you will need to create a directory in ``sensors/src/_your_sensor``, implement a derived class with 
``initialize()``, ``stream_start()``, ``stream_stop()``, ``set_exposure()``, ``configure()`` and ``control()`` methods. When you have your
sensor class implemented you can add its header file into ``sensors/src/sensor_control.cpp`` and have ``sensor_control()`` calling
your sensor API like here:

.. code-block:: C++

  sensor_control(chanend_t c_control) {
    // first i2c_conf definition
    sensor::YOUR_SENSOR snsr(i2c_conf, other_arguments);
    snsr.control(c_control);
  }

After you've done that, you will need to put it into the build system by adding your sources and includes to the following cmake
file ``sensors/CMakeLists.txt``. Then rerunning your build tool (``make`` or ``ninja``) will rerun the cmake and rebuild the project.
