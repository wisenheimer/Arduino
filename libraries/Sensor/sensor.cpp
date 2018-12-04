#include "sensor.h"

/*
    dpin - цифровой пин ардуино
    dtype - тип датчика. Типы татчиков перечислены в "sensor.h"
    start_time_sec = 10 - время на подготовку датчика при старте,
                          когда к нему нельзя обращаться,
                          чтобы не получить ложные данные
*/
Sensor::Sensor(uint8_t dpin, uint8_t dtype, uint8_t start_time_sec = 10)
{
  pin = dpin;
  type = dtype;
  start_time = start_time_sec;
  end_time   = start_time_sec;

  if(type == DHT)
  {
#ifdef SENSOR_DHT_ENABLE
  // Инициализируем датчик температуры и влажности
    dht = new dht11(pin);
    step = DHT_SIGN_ALARM_VALUE; // шаг показаний
#endif
  }
}

Sensor::Sensor(uint8_t dpin, uint8_t dtype, uint8_t pinLevel, uint8_t pin_init_state, uint8_t start_time_sec = 10)
{
  pin = dpin;
  data_pin = dpin;
  type = dtype;

  start_time = start_time_sec;
  end_time   = start_time_sec;

#ifdef SENSOR_DHT_ENABLE
  if(type == DHT)
  {
    // Инициализируем датчик температуры и влажности
    dht = new dht11(DHT_PIN);
    step = DHT_SIGN_ALARM_VALUE; // шаг показаний
    return;
  }
#endif
  pinMode(pin, INPUT); // устанавливаем пин в качестве входа для считывания показаний
  digitalWrite(pin, pin_init_state);
  level = pinLevel;
  prev_pin_state = level;
  count = 0;
  step = ANALOG_SIGN_ALARM_VALUE; // шаг показаний
}

Sensor::~Sensor()
{
#ifdef SENSOR_DHT_ENABLE
  if(type == DHT)
  {
  // Инициализируем датчик температуры и влажности
    delete dht;
  }
#endif
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
  if(type==DHT) return dht->GetData();
#endif
  return analogRead(data_pin);
}

uint8_t Sensor::get_count()  
{
  get_pin_state();  
    
  return count;
}

bool Sensor::analog_sensor_check()
{
  bool res = false;
  uint8_t alarm_value; 
  uint16_t value;

  value = get_data();

  // если показание аналогового датчика превысило пороговое значение
  if(value >= step)
  {
    // увеличиваем это пороговое значение
    if(type == DHT) step=value + 5;
    else step=value + 200;
    // увеличиваем счётчик срабатываний на 1
    count++;
    res = true;
  }

  if(type == DHT) alarm_value = DHT_SIGN_ALARM_VALUE;
  else alarm_value = ANALOG_SIGN_ALARM_VALUE;

  // сброс порогового значения до значения по умолчанию
  if(value < alarm_value)
  {
    if(step>alarm_value)
    {
      step=alarm_value;        
    }      
  }

  return res;
}

// Записываем в строку показания датчика
void Sensor::get_name_for_type(TEXT *str)
{
//  char ch[]={'D','M','R','G','F','H'};
  str->AddChar(' ');
  //str->AddChar(ch[type]);
  switch (type)
  { // Добавляем название датчика
    case DOOR:  str->AddText_P(PSTR("DOOR"));  break;
    case MOVE:  str->AddText_P(PSTR("MOVE"));  break;
    case RADAR: str->AddText_P(PSTR("RADAR")); break;
    case GAS:   str->AddText_P(PSTR("GAS"));   break;
    case FIRE:  str->AddText_P(PSTR("FIRE"));  break;
    case DHT:   str->AddText_P(PSTR("DHT")); break;
  }
  str->AddChar(':');
}

// Записываем в строку показания датчика
void Sensor::get_info(TEXT *str)
{
  get_name_for_type(str);

  switch (type)
  {
#ifdef SENSOR_DHT_ENABLE
    case DHT:
      dht->getInfo(str);      
      break;
#endif
    case GAS:
      str->AddInt(analogRead(data_pin));
      break;
    default:
      bool val = get_pin_state();
      // добавляем число срабатываний датчика
      str->AddInt(count);
      str->AddChar('(');
      // добавляем текущее состояние пина (0 или 1)
      str->AddInt(val);
      str->AddChar(')');
  }
}
