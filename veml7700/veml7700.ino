#include "veml7700.h"

Veml7700* veml;

void setup()
{
  Serial.begin(9600);
  Serial.println("It begins");
  
  veml = new Veml7700();
  veml->init();
  // Gain gain = veml->get_gain();
  // Serial.println(gain);
  // veml->set_integration_time(IntegrationTime::MS25);
  // Serial.println(veml->get_integration_time());

  auto lux = veml->als_to_lux(0b001010111001101, Gain::D4, IntegrationTime::MS100);
  Serial.println(lux);
}

void loop()
{
  float lux;
  lux = veml->get_lux();
  Serial.println(lux);
  delay(10);
}