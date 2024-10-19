#include <Wire.h>

#define CONF0_SET_SHUTDOWN(val, bit_value)  ((val) = ((val) & ~1) | (bit_value))
#define CONF0_SET_ENABLE_INTERRUPT(val, bit_value)  ((val) = ((val) & ~(1 << 1)) | ((bit_value) << 1))
#define CONF0_SET_PERSISTENCE(val, bit_value)  ((val) = ((val) & ~(3 << 4)) | ((bit_value & 3) << 4))
#define CONF0_SET_INTERGRATION_TIME(val, bit_value)  ((val) = ((val) & ~(0xF << 6)) | ((bit_value & 0xF) << 6))
#define CONF0_SET_GAIN(val, bit_value)  ((val) = ((val) & ~(3 << 11)) | ((bit_value & 3) << 11))

#define VEML7700_I2C_ADDR 0x10

typedef enum
{
  CONF_0      = 0x0, 
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
}IntergrationTime;

typedef enum
{
  _1 = 0x0,
  _2 = 0x1,
  _4 = 0x2,
  _8 = 0x3
}Persistence;

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

void veml7700_init()
{
  Wire.begin();
  uint16_t conf_0 = 0;
  CONF0_SET_SHUTDOWN(conf_0, 0);
  CONF0_SET_ENABLE_INTERRUPT(conf_0, 0);
  CONF0_SET_PERSISTENCE(conf_0, IntergrationTime::MS100);
  CONF0_SET_INTERGRATION_TIME(conf_0, Persistence::_1);
  CONF0_SET_GAIN(conf_0, Gain::X2);
  
  veml7700_send(conf_0, Register::CONF_0);
  veml7700_send(0, Register::THRESHOLD_L);
  veml7700_send(0xffff, Register::THRESHOLD_H);
  delay(3);
}

void veml7700_send(uint32_t data, uint8_t reg)
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

uint32_t veml7700_receive(uint8_t reg)
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

void setup()
{
  Serial.begin(9600);
  Serial.println("It begins");
  veml7700_init();
}

void loop()
{
  float lux;
  lux = veml7700_receive(Register::ALS);
  Serial.println(lux);
  delay(10);
}