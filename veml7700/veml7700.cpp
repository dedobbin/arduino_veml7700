#include <Arduino.h>
#include <Wire.h>
#include "veml7700.h"

void error(String msg)
{
  if (Serial.availableForWrite() > 0) {
      Serial.println(msg);
  }
  while(1){}
}

void check_transmission_result(uint8_t res)
{
  switch (res){
    case 0:
      //Serial.println("Result is OK");
      break;
    case 1:
      error("Too much data for buffer");
      break;
    case 2:
      error("NACK on address transmission");
      break;
    case 3:
      error("NACK on data transmission");
      break;
    case 4:
      error("Other i2c error");
      break;
    default:
      error("unknown error");
      break;
  }
}

void Veml7700::init()
{
  Wire.begin();
  CONF0_SET_SHUTDOWN(conf0_cache, 0);
  CONF0_SET_ENABLE_INTERRUPT(conf0_cache, 0);
  CONF0_SET_PERSISTENCE(conf0_cache, Persistence::_1);
  CONF0_SET_INTEGRATION_TIME(conf0_cache, IntegrationTime::MS100);
  CONF0_SET_GAIN(conf0_cache, Gain::X1);
  send(conf0_cache, Register::CONF0);
  
  delay(3); // Should be done after setting power mode

  send(0, Register::THRESHOLD_L);
  send(0xffff, Register::THRESHOLD_H);
}

void Veml7700::set_gain(Gain gain)
{
  CONF0_SET_GAIN(conf0_cache, gain);
  send(conf0_cache, Register::CONF0);
}

void Veml7700::set_integration_time(IntegrationTime time)
{
  CONF0_SET_INTEGRATION_TIME(conf0_cache, time);
  send(conf0_cache, Register::CONF0);
}

float Veml7700::get_lux() const
{
  uint32_t als_value = receive(Register::ALS);
  return als_to_lux(als_value, get_gain(), get_integration_time());
}

Gain Veml7700::get_gain() const
{
  return CONF0_GET_GAIN(conf0_cache);
}

IntegrationTime Veml7700::get_integration_time() const
{
  return CONF0_GET_INTEGRATION_TIME(conf0_cache);
}

void Veml7700::refresh_conf0_cache()
{
  conf0_cache = receive(Register::CONF0);
}

void Veml7700::send(uint32_t data, uint8_t reg) const
{
  Wire.beginTransmission(VEML7700_I2C_ADDR);
  if (Wire.write(reg) != 1) {
    error("failed to write addr");
  }

  if (Wire.write(uint8_t(data & 0xff)) != 1) {
    error("failed to write");
  }

  if (Wire.write(uint8_t(data >> 8)) != 1) {
    error("failed to write");
  }

  uint8_t transmission_res = Wire.endTransmission();
  check_transmission_result(transmission_res);
}

uint32_t Veml7700::receive(uint8_t reg) const
{
  Wire.beginTransmission(VEML7700_I2C_ADDR);
  if (Wire.write(reg) != 1) {
    error("failed to write");
  }

  uint8_t transmission_res = Wire.endTransmission(false); 
  check_transmission_result (transmission_res);
  
  if (Wire.requestFrom(uint8_t(VEML7700_I2C_ADDR), uint8_t(2)) != 2) {
    error("request unserviceable");
  }
  uint32_t data = Wire.read();
  data |= uint32_t(Wire.read()) << 8;
  return data;
}

float Veml7700::als_to_lux(uint16_t als_value, Gain gain, IntegrationTime integration_time) const
{
  // TODO: MAXIMUM POSSIBLE ILLUMINATION (lx)
  // https://www.vishay.com/docs/84323/designingveml7700.pdf
  float resolution = 0.f;
  if (gain == Gain::X2)
     resolution = 0.0042f;
  else if (gain == Gain::X1)
    resolution = 0.0084f;
  else if (gain== Gain::D4)
    resolution = 0.0336;
  else if (gain == Gain::D8)
    resolution = 0.0672;
  else
    error("Unknown gain");
  
  if (integration_time == IntegrationTime::MS800)
    resolution *= 1;
  else if (integration_time == IntegrationTime::MS400)
    resolution *= 2;
  else if (integration_time == IntegrationTime::MS200)
    resolution *= 4;
  else if (integration_time == IntegrationTime::MS100)
    resolution *= 8;
  else if (integration_time == IntegrationTime::MS50)
    resolution *= 16;
  else if (integration_time == IntegrationTime::MS25)
    resolution *= 32;
  else 
    error("Unknown integration time");
  // TODO: for illuminations > 1000 lx a correction formula needs to be applied
  // TODO: When using GAIN level 1/4 and 1/8 the correction formula should be used.
  return als_value * resolution;
}
