/*
 * IRsensor: sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1, 2019
 * Copyright 2019
 */

#include <IRremote.h>
#include "my_sensors.h"

IRsend irsend;

#if WTD_ENABLE

#include <avr/wdt.h>
#include <stdint.h>

uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
void get_mcusr(void) \
  __attribute__((naked)) \
  __attribute__((used)) \
  __attribute__((section(".init3")));
void get_mcusr(void)
{
  mcusr_mirror = MCUSR;
  MCUSR = 0;
  wdt_disable();
}

#endif

// активируем флаг тревоги для сбора информации и отправки ик кода на сигнализацию
#define ALARM_ON  if(!GET_FLAG(ALARM)){SET_FLAG_ONE(ALARM);AlarmTime=ALARM_MAX_TIME;}
// по окончании времени ALARM_MAX_TIME обнуляем флаг тревоги
#define ALARM_OFF {SET_FLAG_ZERO(ALARM);sensors->Clear();}

static uint32_t msec = 0;
static uint8_t AlarmTime = 0;
uint8_t flags = 0;

MY_SENS *sensors = NULL;

void setup()
{
#if WTD_ENABLE
  wdt_disable();
  wdt_enable(WDTO_8S);
#endif

  sensors = new MY_SENS();
}

void timer(uint16_t time)
{
  if(millis() - msec >= time)
  {
    msec = millis();

    DEBUG_PRINT('.');
    
	// Опрос датчиков ///
    if(sensors->SensOpros())
    {
      ALARM_ON // режим тревога вкл.
      irsend.sendNEC(IR_CODE, IR_CODE_BIT_SIZE);
    }
    if(GET_FLAG(ALARM))
    {
      if(AlarmTime) AlarmTime--;
      else // По истечении заданного времени ALARM_MAX_TIME 
      {
        ALARM_OFF; // Выключаем режим тревоги. Очищаем статистику датчиков.            
      }
    } 
  }
}

void loop()
{
  while(1)
  {
#if WTD_ENABLE
    wdt_reset();
#endif    
    timer(1000);
  }    
}
