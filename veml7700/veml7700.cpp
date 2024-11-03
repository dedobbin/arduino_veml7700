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

void Veml7700::begin()
{
  Wire.begin();
  CONF0_SET_SHUTDOWN(conf_0_cache, 0);
  CONF0_SET_ENABLE_INTERRUPT(conf_0_cache, 0);
  CONF0_SET_PERSISTENCE(conf_0_cache, IntergrationTime::MS100);
  CONF0_SET_INTERGRATION_TIME(conf_0_cache, Persistence::_1);
  CONF0_SET_GAIN(conf_0_cache, Gain::X2);
  send(conf_0_cache, Register::CONF_0);

  send(0, Register::THRESHOLD_L);
  send(0xffff, Register::THRESHOLD_H);
  delay(3);
}

void Veml7700::send(uint32_t data, uint8_t reg)
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

uint32_t Veml7700::receive(uint8_t reg)
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

