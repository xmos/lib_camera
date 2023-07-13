
#include <cstdint>

#include "imx219_reg.h"
#include "imx219_i2c.h"

namespace imx219 {


typedef enum {
  RES_640_480,
  RES_1640_1232,
} resolution_t;

template<resolution_t TRES>
imx219_settings_t* get_res_array();

typedef enum {
  FMT_RAW8,
  FMT_RAW10
} pixel_format_t;

template<pixel_format_t TFMT>
imx219_settings_t* get_pxl_fmt_array();



class SensorBase {

  public:

    static constexpr uint8_t BLACK_LEVEL = 16;

  public:

    unsigned i2c_client;

    SensorBase(unsigned i2c_client)
        : i2c_client(i2c_client) {}

    int initialize();
    int stream_start();
    int stream_stop();

    int set_exposure(uint32_t dBGain);

    int read(uint16_t addr);
    void read_gains(uint16_t values[5]);
    
    int configure(imx219_settings_t* resolution,
                  imx219_settings_t* pixel_format);
};




template <resolution_t TRES, pixel_format_t TFMT>
class IMX219Sensor : public SensorBase {

  public:

    IMX219Sensor(unsigned i2c_client)
        : SensorBase(i2c_client) {}

    int configure()
    {
      imx219_settings_t* res_setting = get_res_array<TRES>();
      imx219_settings_t* fmt_settings = get_pxl_fmt_array<TFMT>();
      return this->configure(res_setting, fmt_settings);
    }
};



} // namespace imx219