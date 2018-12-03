#include "beep.h"

static uint8_t pin;

uint32_t buzz_time = 0;

struct buzzer
{
  uint16_t freq_hz;
  uint16_t msec;
  uint16_t pause;  
} buzz[NO_BEEP] = 
{
  5000, 1000, 1000,
  1000, 400,  200
};

void beep(uint8_t mode)
{
  if(mode != NO_BEEP)
  {
    if(millis() - buzz_time > buzz[mode].pause)
    {
      tone(pin, buzz[mode].freq_hz);
      delay(buzz[mode].msec);
      noTone(pin);
      buzz_time = millis();
    }
  }
}

void beep_init(uint8_t buzzer_pin)
{
  pin = buzzer_pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  //beep(DEFAULT_BEEP);
}

void noBeep()
{
  noTone(pin);  
}
