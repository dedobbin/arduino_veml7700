#include "veml7700.h"

Veml7700* veml;

void setup()
{
  Serial.begin(9600);
  Serial.println("It begins");
  
  veml = new Veml7700();
  veml->init();
  veml->enable_interrupt();

  // auto lux = veml->als_to_lux(0b001010111001101, Gain::D4, IntegrationTime::MS100);
  // Serial.println(lux);
}

void loop()
{
  // float lux;
  // lux = veml->get_lux();
  // Serial.println(lux);

  uint32_t interrupt_status = veml->receive(Register::INTERRUPT);
  if (interrupt_status & InterruptStatus::INTERRUPT_HIGH){
    Serial.println("High");
  }

  if (interrupt_status & InterruptStatus::INTERRUPT_LOW){
    Serial.println("Low");
  }
  delay(10);
}