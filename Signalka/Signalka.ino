#include "modem.h"

#if WTD_ENABLE

  #include <avr/wdt.h>
  #include <stdint.h>

  uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
  void get_mcusr(void)  \
  __attribute__((naked))\
  __attribute__((used)) \
  __attribute__((section(".init3")));

  void get_mcusr(void)
  {
    mcusr_mirror = MCUSR;
    MCUSR = 0;
    wdt_disable();
  }

#endif

static uint32_t msec = 0;
static uint8_t AlarmTime = 0;
extern uint8_t flags;

MODEM *phone;
MY_SENS *sensors = NULL;

// активируем флаг тревоги для сбора информации и отправки e-mail
#define ALARM_ON  if(!GET_FLAG(ALARM)){SET_FLAG_ONE(ALARM);AlarmTime=ALARM_MAX_TIME;phone->ring_to_admin(); \
                  phone->email_buffer->AddText_P(PSTR(" ALARM!"));sensors->GetInfo(phone->email_buffer);DEBUG_PRINTLN(phone->email_buffer->GetText());}
// по окончании времени ALARM_MAX_TIME обнуляем флаг тревоги и отправляем e-mail с показаниями датчиков
#define ALARM_OFF {SET_FLAG_ZERO(ALARM);if(GET_FLAG(GUARD_ENABLE)){phone->email_buffer->AddText_P(PSTR(" ALL:"));sensors->GetInfo(phone->email_buffer); \
                  sensors->Clear();DEBUG_PRINTLN(phone->email_buffer->GetText());}}

void power()
{
  SET_FLAG_ONE(INTERRUPT);
}

void setup()
{
#if WTD_ENABLE
  wdt_disable();
  wdt_enable(WDTO_8S);
#endif

  pinMode(RING_PIN, INPUT);
  digitalWrite(RING_PIN, LOW);
  pinMode(POWER_PIN, INPUT);
  digitalWrite(POWER_PIN, LOW);
  pinMode(BOOT_PIN, OUTPUT);
  digitalWrite(BOOT_PIN, LOW);

  // Прерывание для POWER_PIN
  attachInterrupt(1, power, CHANGE);

  phone = new MODEM();

  phone->init();
}

void timer(uint16_t time)
{
  if(millis() - msec >= time)
  {
    msec = millis();

    DEBUG_PRINT('.');
    
    if(GET_FLAG(GUARD_ENABLE))
    { /// Опрос датчиков ///
      if(sensors->SensOpros())
      {
        ALARM_ON // режим тревога вкл.                            
      }       
    }
    if(GET_FLAG(ALARM))
    {
      if(AlarmTime) AlarmTime--;
      else // По истечении заданного времени ALARM_MAX_TIME 
      {
        ALARM_OFF; // Выключаем режим тревоги и отправляем e-mail. Очищаем статистику датчиков.            
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
    phone->wiring(); // слушаем модем
    timer(1000);
  }    
}
