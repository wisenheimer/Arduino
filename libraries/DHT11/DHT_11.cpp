#include "DHT_11.h"

dht11::dht11(uint8_t dpin)
{
  pin = dpin;

  sens = new DHT(DHT11);
}

uint8_t dht11::GetData()
{
  return sens->readTemperature(pin);
}

void dht11::getInfo(TEXT* str)
{
  str->AddText_P(PSTR("t="));
  str->AddInt(sens->readTemperature(pin));
  str->AddText_P(PSTR("*C,h="));
  str->AddInt(sens->readHumidity(pin));
  str->AddChar('%');
}
