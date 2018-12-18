#include "sensor.h"

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
                uint8_t alarm_val = 200)
{
  pin = _pin;
  type = _type;
  name = (char*)calloc(1,strlen(sens_name)+1);
  strcpy(name, sens_name);

  start_time = start_time_sec;
  end_time   = start_time_sec;
    
  alarm_value = alarm_val;
  count = 0;

#ifdef DHT_ENABLE
  if(type == DHT11 || type == DHT21 || type == DHT22)
  {
    // Инициализируем датчик температуры и влажности
    dht = new DHT(type, pin);
    return;
  }
#endif
  pinMode(pin, INPUT); // устанавливаем пин в качестве входа для считывания показаний
  digitalWrite(pin, LOW);
  level = pinLevel;
  prev_pin_state = level;   
}

Sensor::~Sensor()
{
#ifdef DHT_ENABLE
  if(type == DHT11 || type == DHT21 || type == DHT22)
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
      if(type==CHECK_DIGITAL_SENSOR) start_time = 10; // опросим повторно через 10 сек
      else count++;
    }
    else if(type==CHECK_DIGITAL_SENSOR) count++; 
  }

  prev_pin_state = state;
  
  return state;
}

uint8_t Sensor::get_count()  
{
  int16_t val;
  
  switch (type)
  {
    case DIGITAL_SENSOR:
    case CHECK_DIGITAL_SENSOR:
      get_pin_state();
      break;
    default:
      start_time = 10;
      // если показание аналогового датчика превысило пороговое значение

#ifdef DHT_ENABLE
      if(type == DHT11 || type == DHT21 || type == DHT22) val = dht->readTemperature();
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
#ifdef DHT_ENABLE
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
    case ANALOG_SENSOR:
      str->AddInt(analogRead(pin));
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
}
