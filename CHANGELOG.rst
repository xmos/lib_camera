fwk_camera change log
=====================

0.2.0
-----
* CHANGED: Mipi has been reimplemented to C

0.1.0
-----

* ADDED: Sony IMX219 support (Raspberry Pi Camera V2).
* ADDED: RAW8 and Raw10 capture support using only internal RAM.
* ADDED: Raw8 capture example (640X480).
* ADDED: Raw8 + Downsample exmaple (160x120x3) RGB.
* ADDED: Xscope_fileio suport.
* ADDED: AE control: auto exposure based on histogram skewness.
* ADDED: AWB control: auto white balance. Combination of grey world assumption and percentile white balance. 
* ADDED: AWB control: auto white balance based on a combination of grey world assumption and percentile.
* ADDED: Image rotation: image rotation capabilities (sensor and ISP)
* ADDED: cropping / scaling : image scalling, cropping. 
* ADDED: Image statistics: histogram, mean, variance, skewness. 
* ADDED: sensor control, start, stop functons. 
