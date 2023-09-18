fwk_camera change log
=====================

0.2.1
-----

  * CHANGED: Small documentation update

0.2.0
-----

  * ADDED: Dynamic configuration for the resolution, pixel format, binning mode
    and frame offsets
  * CHANGED: Mipi has been reimplemented to C
  * CHANGED: I2c library version (from xc version to c version from fwk_io)
  * CHANGED: Sensor control has been reimplemented
  * CHANGED: "assert" function replaced with "xassert" from <xcore/assert.h>
  * CHANGED: streaming channels of camera_api.c have been replaced with channels
  * ADDED: new supported mode, MODE_1280x960
  * CHANGED: value of  PLL_VT_MPY from 0x0047 to 0x0027 due to timing in
    MODE_1280x960
  * CHANGED: default AWB static values in order to match sony sensor

0.1.0
-----

  * ADDED: Sony IMX219 support (Raspberry Pi Camera V2).
  * ADDED: RAW8 and Raw10 capture support using only internal RAM.
  * ADDED: Raw8 capture example (640X480).
  * ADDED: Raw8 + Downsample exmaple (160x120x3) RGB.
  * ADDED: Xscope_fileio suport.
  * ADDED: AE control: auto exposure based on histogram skewness.
  * ADDED: AWB control: auto white balance. Combination of grey world assumption
    and percentile white balance.
  * ADDED: AWB control: auto white balance based on a combination of grey world
    assumption and percentile.
  * ADDED: Image rotation: image rotation capabilities (sensor and ISP)
  * ADDED: cropping / scaling : image scalling, cropping.
  * ADDED: Image statistics: histogram, mean, variance, skewness.
  * ADDED: sensor control, start, stop functons.

