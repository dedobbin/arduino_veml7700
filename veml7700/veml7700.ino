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
  veml->set_gain(Gain::X1);
  gain = veml->get_gain();
  Serial.println(gain);

}

void loop()
{
  // float lux;
  // lux = veml->receive(Register::ALS);
  // Serial.println(lux);
  // delay(10);
}