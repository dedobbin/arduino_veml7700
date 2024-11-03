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
  veml->set_integration_time(IntegrationTime::MS25);
  veml->refresh_conf0_cache();
  Serial.println(veml->get_integration_time());
}

void loop()
{
  float lux;
  lux = veml->get_lux();
  Serial.println(lux);
  delay(10);
}