#pragma once

#include <stdint.h>

#define CONF0_SET_SHUTDOWN(val, bit_value)  ((val) = ((val) & ~1) | (bit_value))
#define CONF0_SET_ENABLE_INTERRUPT(val, bit_value)  ((val) = ((val) & ~(1 << 1)) | ((bit_value) << 1))
#define CONF0_SET_PERSISTENCE(val, bit_value)  ((val) = ((val) & ~(3 << 4)) | ((bit_value & 3) << 4))
#define CONF0_SET_INTEGRATION_TIME(val, bit_value)  ((val) = ((val) & ~(0xF << 6)) | ((bit_value & 0xF) << 6))
#define CONF0_SET_GAIN(val, bit_value)  ((val) = ((val) & ~(3 << 11)) | ((bit_value & 3) << 11))

#define CONF0_GET_SHUTDOWN(val)          ((val) & 1)
#define CONF0_GET_ENABLE_INTERRUPT(val)  (((val) >> 1) & 1)
#define CONF0_GET_PERSISTENCE(val)       (((val) >> 4) & 3)
#define CONF0_GET_INTEGRATION_TIME(val)  (((val) >> 6) & 0xF)
#define CONF0_GET_GAIN(val)              (((val) >> 11) & 3)

#define VEML7700_I2C_ADDR 0x10

typedef enum
{
  CONF0      = 0x0, 
  THRESHOLD_H = 0x01,
  THRESHOLD_L = 0x02,
  ALS         = 0x04
}Register;

typedef enum 
{
  X1 = 0x0,
  X2 = 0x1,
  D8 = 0x2,
  D4 = 0x3
}Gain;

typedef enum
{
  MS25    = 0xc,
  MS50    = 0x8,
  MS100   = 0x0,
  MS200   = 0x1,
  MS400   = 0x2,
  MS800   = 0x3
}IntegrationTime;

typedef enum
{
  _1 = 0x0,
  _2 = 0x1,
  _4 = 0x2,
  _8 = 0x3
}Persistence;


class Veml7700
{
  // TODO:  At lux levels above 100 lux GAIN level 1 and 2 should not be used as the sensor becomes non-linear
  // https://www.vishay.com/docs/84323/designingveml7700.pdf
public:
  void init();

  void set_gain(Gain gain);
  void set_integration_time(IntegrationTime time);

  float get_lux() const;
  Gain get_gain() const;
  IntegrationTime get_integration_time() const;

  void refresh_conf0_cache();

  float als_to_lux(uint16_t als_value, Gain gain, IntegrationTime integration_time) const;
  float lux_correction(float lux) const;
private:
  void send(uint32_t data, uint8_t reg) const;
  uint32_t Veml7700::receive(uint8_t reg) const;

  uint16_t conf0_cache = 0;
};