#include "modem.h"
#include "my_sensors.h"

static uint8_t AlarmTime = 0;

extern uint8_t flags;

static uint32_t msec = 0;

MODEM *phone;

MY_SENS *sensors = NULL;

// активируем флаг тревоги для сбора информации и отправки e-mail
#define ALARM_ON  if(!GET_FLAG(ALARM)){sensors->SaveEnableTmp();SET_FLAG_ONE(ALARM);AlarmTime=ALARM_MAX_TIME;phone->ring_to_admin(); \
                  phone->email_buffer->AddText_P(PSTR(" ALARM!"));sensors->GetInfo(phone->email_buffer);}
// по окончании времени ALARM_MAX_TIME обнуляем флаг тревоги и отправляем e-mail с показаниями датчиков
#define ALARM_OFF {SET_FLAG_ZERO(ALARM);sensors->RestoreEnable();phone->email_buffer->AddText_P(PSTR(" ALL:"));sensors->GetInfo(phone->email_buffer);sensors->Clear();}

// Переменные, создаваемые процессом сборки,
// когда компилируется скетч
extern int __bss_end;
extern void *__brkval;

// Функция, возвращающая количество свободного ОЗУ (RAM)
int memoryFree()
{
   int freeValue;
   if((int)__brkval == 0)
      freeValue = ((int)&freeValue) - ((int)&__bss_end);
   else
      freeValue = ((int)&freeValue) - ((int)__brkval);
   return freeValue;
}

//****************обработчик прерывания********************

void power()
{
  if(digitalRead(POWER_PIN))
  {
    sensors->TimeReset();
    phone->email_buffer->AddText_P(PSTR(" Svet ON."));
  }
  else phone->email_buffer->AddText_P(PSTR(" Svet OFF."));  
} 

//**********************************************************

void setup()
{
  pinMode(POWER_PIN, INPUT);
  digitalWrite(POWER_PIN, LOW);
  pinMode(BOOT_PIN, OUTPUT);
  digitalWrite(BOOT_PIN, LOW);

  // Прерывание для POWER_PIN
  attachInterrupt(1, power, CHANGE);

  sensors = new MY_SENS();
  
  // Подтянем пин к 5 вольтам, чтобы датчик двери мог сработать
  // при отключении внешнего питания
  pinMode(DOOR_PIN, INPUT_PULLUP);

  phone = new MODEM(sensors->flag_enable);

  phone->init();

#ifdef DEBUG_MODE
  Serial.print(F("RAM free:"));
  Serial.println(memoryFree()); // печать количества свободной оперативной памяти
#endif
}

void timer(uint16_t time)
{
  if(millis() - msec >= time)
  {
    msec = millis();
#ifdef DEBUG_MODE
    Serial.print('.');
#endif
    
    if(GET_FLAG(GUARD_ENABLE))
    {      
      /// Опрос датчиков ///
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
  phone->wiring(); // слушаем модем

  timer(1000);  
}
