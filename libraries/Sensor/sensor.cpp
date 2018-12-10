#include "sensor.h"

/*
    _pin - пин ардуино
    _type - типы датчиков перечислены в "sensor.h"
    sens_name - имя датчика
    pinLevel - уровень на пине датчика в спокойном режиме (LOW или HIGHT)
    start_time_sec = 10 - время на подготовку датчика при старте,
                          когда к нему нельзя обращаться,
                          чтобы не получить ложные данные
    alarm_val = ANALOG_SIGN_ALARM_VALUE - значение срабатывания аналогового датчика
*/
Sensor::Sensor(uint8_t _pin, uint8_t _type, char* sens_name, uint8_t pinLevel = LOW, uint8_t start_time_sec = 10, uint8_t alarm_val = ANALOG_SIGN_ALARM_VALUE)
{
  pin = _pin;
  type = _type;
  name = (char*)calloc(1,strlen(sens_name)+1);
  strcpy(name, sens_name);

  start_time = start_time_sec;
  end_time   = start_time_sec;
    
  alarm_value = alarm_val;
  count = 0;

#ifdef SENSOR_DHT_ENABLE
  if(type == DHT11 || type == DHT21 || type == DHT22)
  {
    // Инициализируем датчик температуры и влажности
    dht = new DHT(type);
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
#ifdef SENSOR_DHT_ENABLE
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
  bool state = false;
   
  state = digitalRead(pin);

  if(level)
  {
      if(!state && prev_pin_state) count++;
  }
  else if(state && !prev_pin_state) count++;		

  prev_pin_state = state;
  
  return state;
}

uint16_t Sensor::get_data()
{
#ifdef SENSOR_DHT_ENABLE
  if(type == DHT11 || type == DHT21 || type == DHT22) return dht->readTemperature(pin);
#endif
  return analogRead(pin);
}

uint8_t Sensor::get_count()  
{
  get_pin_state();  
    
  return count;
}

bool Sensor::get_analog_count()
{
  if(count) return true;
    // если показание аналогового датчика превысило пороговое значение
  if(get_data() >= alarm_value)
  {
    // увеличиваем счётчик срабатываний на 1
    count++;
    return true;
  }
  return false;
}

// Записываем в строку показания датчика
void Sensor::get_info(TEXT *str)
{
  str->AddText(name);
  str->AddChar(':');

  switch (type)
  {
#ifdef SENSOR_DHT_ENABLE
    case DHT11:
    case DHT21:
    case DHT22:
      str->AddText_P(PSTR("t="));
      str->AddInt(dht->readTemperature(pin));
      str->AddText_P(PSTR("*C,h="));
      str->AddInt(dht->readHumidity(pin));
      str->AddChar('%');     
      break;
#endif
    case ANALOG_SENSOR:
      str->AddInt(analogRead(pin));
      break;
    default:
      char val = get_pin_state() + 48;
      // добавляем число срабатываний датчика
      str->AddInt(count);
      str->AddChar('(');
      // добавляем текущее состояние пина (0 или 1)
      str->AddChar(val);
      str->AddChar(')');
  }
}
