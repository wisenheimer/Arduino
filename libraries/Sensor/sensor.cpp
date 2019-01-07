#include "sensor.h"
#include "termistor.h"

Sensor::Sensor( // типы датчиков перечислены в "sensor.h"
                uint8_t _type,
                // имя датчика
                char* sens_name,
                // ик код
                uint32_t ir_code)
{
  type = _type;
  name = (char*)malloc(strlen(sens_name)+1);
  strcpy(name, sens_name);
  alarm_value = ir_code;
  count = 0;
#if IR_ENABLE
  IRrecvInit();
#endif 
}

Sensor::Sensor( // пин ардуино
                uint8_t _pin,
                // типы датчиков перечислены в "sensor.h"
                uint8_t _type,
                // имя датчика
                char* sens_name,
                // уровень на пине датчика в спокойном режиме (LOW или HIGHT)
                uint8_t pinLevel = LOW,
                // время на подготовку датчика при старте
                uint8_t start_time_sec = 10,
                // значение срабатывания аналогового датчика
                uint32_t alarm_val = 200)
{
  pin = _pin;
  type = _type;
  name = (char*)malloc(strlen(sens_name)+1);
  strcpy(name, sens_name);
  
  start_time = start_time_sec;
  end_time   = start_time_sec;
    
  alarm_value = alarm_val;
  count = 0;
  check = false;

#if DHT_ENABLE
  if(type >= DHT11 && type <= DHT22)
  {
    // Инициализируем датчик температуры и влажности
    dht = new DHT(type, pin);
    return;
  }
#endif
  pinMode(pin, INPUT); // устанавливаем пин в качестве входа для считывания показаний
  if(type == TERMISTOR) return;
  digitalWrite(pin, LOW);
  level = pinLevel;
  prev_pin_state = level;   
}

Sensor::~Sensor()
{
#if DHT_ENABLE
  if(type >= DHT11 && type <= DHT22)
  {
  // Инициализируем датчик температуры и влажности
    delete dht;
  }
#endif
  free(name);
}

bool Sensor::get_pin_state()
{
  bool state = digitalRead(pin);

  if(level!=state)
  {
    if(level==prev_pin_state)
    {
      // если сенсор сработал, исключаем ложное срабатывание датчика
      if(type==CHECK_DIGITAL_SENSOR)
      {
        start_time = 10; // опросим повторно через 10 сек
        check = true;
      } 
      else count++;
    }
    else if(check)
    {
      check = false;
      count++;
    }  
  }

  prev_pin_state = state;
  
  return state;
}

uint8_t Sensor::get_count()  
{
  int16_t val;
  
  switch (type)
  {
#if IR_ENABLE
    case IR_SENSOR:
      if(alarm_value == IRgetValue()) count++;
      break;
#endif
    case DIGITAL_SENSOR:
    case CHECK_DIGITAL_SENSOR:
      get_pin_state();
      break;
    default:
      start_time = 10;
      // если показание аналогового датчика превысило пороговое значение

#if DHT_ENABLE
      if(type == DHT11 || type == DHT21 || type == DHT22) val = dht->readTemperature();
      else
#endif

#if TERM_ENABLE
      if(type == TERMISTOR) val = readTemperature(pin);
      else
#endif
      val = analogRead(pin);

      if(val >= alarm_value) count++;                 
  }   
    
  return count;
}

// Записываем в строку показания датчика
void Sensor::get_info(TEXT *str)
{
  str->AddText(name);
  str->AddChar(':');

  switch (type)
  {
    case ANALOG_SENSOR:
      str->AddInt(analogRead(pin));
      break;
    
#if DHT_ENABLE
    case DHT11:
    case DHT21:
    case DHT22:
      str->AddText_P(PSTR("t="));
      str->AddInt(dht->readTemperature());
      str->AddText_P(PSTR("C,h="));
      str->AddInt(dht->readHumidity());
      str->AddChar('%');
      break;
#endif

#if TERM_ENABLE
    case TERMISTOR:
      str->AddText_P(PSTR("t="));
      str->AddInt(readTemperature(pin));
      str->AddChar('C');  
      break;
#endif

    case IR_SENSOR:
      // добавляем число срабатываний датчика
      str->AddInt(count);
      break;

    default:
      char val = get_pin_state() + '0';
      // добавляем число срабатываний датчика
      str->AddInt(count);
      str->AddChar('(');
      // добавляем текущее состояние пина (0 или 1)
      str->AddChar(val);
      str->AddChar(')');
  }
  str->AddChar(' ');
}
