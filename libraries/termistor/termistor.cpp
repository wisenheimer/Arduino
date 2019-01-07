#include "termistor.h"
#include <Arduino.h>

//////////////////////////////////////////////////////////
/// Настройки термистора (если используется)
//////////////////////////////////////////////////////////
// бета коэффициент термистора (обычно 3000-4000)
#define BCOEFFICIENT 3950
// температура, при которой замерено номинальное сопротивление
#define TEMPERATURENOMINAL 25
// сопротивление при 25 градусах по Цельсию
#define THERMISTOR_OM 10000
// сопротивление второго резистора
#define RESISTOR_OM 9850

float readTemperature(int pin) 
{      
  float f = analogRead(pin);
  // конвертируем значение в сопротивление
  f = 1023 / f - 1;
  f = RESISTOR_OM / f;
  f /= THERMISTOR_OM; // (R/Ro)
  f = log(f); // ln(R/Ro)
  f /= BCOEFFICIENT; // 1/B * ln(R/Ro)
  f += 1.0 /(TEMPERATURENOMINAL + 273.15); // + (1/To)
  f = 1.0 / f; // инвертируем
  f -= 273.15; // конвертируем в градусы по Цельсию
    
  return f;
}
