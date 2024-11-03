#include "veml7700.h"

Veml7700* veml;

void setup()
{
  Serial.begin(9600);
  Serial.println("It begins");
  
  veml = new Veml7700();
  veml->init();
  Gain gain = veml->get_gain();
  Serial.println(gain);
  veml->set_gain(Gain::D8);
  gain = veml->get_gain();
  Serial.println(gain);
  veml->set_intergration_time(IntergrationTime::MS25);
  veml->refresh_conf0_cache();
  Serial.println(veml->get_intergration_time());
}

void loop()
{
  // float lux;
  // lux = veml->receive(Register::ALS);
  // Serial.println(lux);
  // delay(10);
}