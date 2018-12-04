
#include "my_sensors.h"

MY_SENS::MY_SENS()
{
	// Флаг flag_enable разрешает датчикам быть источником для включения режима тревоги.
	// Каждый бит соответсвует подключенному сенсору, в том порядке, как ини записаны в
	// в файле "my_sensors.h" при инициализации класса Sensor sensors.
	// Например первый бит - это геркон, второй - радар, третий - датчик движения и т.д.
	// Если в бит записать 0, то показания датчика будут учитываться только когда наступил режим тревоги,
	// но сам он не сможет его вызвать. 
	// Это сделано по той причине, что радар реагирует на соседей за стенкой, а PIR датчик, находясь вблизи
	// от модема, может давать ложные срабатывания от излучения антенны. Чтобы это исключить, можно записать
	// flag_enable=0b11111001; Тогда при срабатывании геркона мы можем учитывать показания датчиков движения,
	// как подтверждение, что на объекте кто-то есть.
	flag_enable=0b11111111;
}

MY_SENS::~MY_SENS()
{
  
}

void MY_SENS::GetInfo(TEXT *buf)
{
  uint8_t size = sizeof(sensors)/sizeof(Sensor);
  for(uint8_t i=0;i<size;i++)
  {
    // Выводим только сработавшие датчики
    if(sensors[i].count || sensors[i].type==DHT || sensors[i].type==GAS)
    {
      sensors[i].get_info(buf);
    }
  }

  buf->AddChar('\n');
}

void MY_SENS::Clear()
{
	uint8_t size = sizeof(sensors)/sizeof(Sensor);
  for(uint8_t i=0;i<size;i++) sensors[i].count=0;
}

bool MY_SENS::ReadPin(uint8_t sens_index)
{
	return sensors[sens_index].get_pin_state();
}

uint8_t MY_SENS::Count(uint8_t sens_index)
{
	return sensors[sens_index].count;
}

void MY_SENS::SetOne(uint8_t sens_index)
{
	bitSet(flag_enable,sens_index);
}

void MY_SENS::SetZero(uint8_t sens_index)
{
	bitClear(flag_enable,sens_index);
}

bool MY_SENS::Enable(uint8_t sens_index)
{
	return bitRead(flag_enable,sens_index);
}

void MY_SENS::SaveEnableTmp()
{
	tmp=flag_enable;
  flag_enable=0xFF;
}

void MY_SENS::RestoreEnable()
{
	flag_enable = tmp;
}

uint8_t MY_SENS::check_sens_fire(uint8_t sens_index)
{      
  uint8_t count = sensors[sens_index].count;

  if(sensors[sens_index].get_count() > count)
  {
    // если сенсор сработал, исключаем ложное срабатывание датчика
    DELAY(10000);
    if(digitalRead(sensors[sens_index].pin) == sensors[sens_index].level) sensors[sens_index].count = count;
  } 

  return sensors[sens_index].count;
}

void MY_SENS::TimeReset()
{      
  uint8_t size = sizeof(sensors)/sizeof(Sensor);

  for(uint8_t i = 0; i < size; i++)
  {
    sensors[i].start_time = sensors[i].end_time;
  }
}

uint8_t MY_SENS::SensOpros()
{
  uint8_t count = 0;

  uint8_t size = sizeof(sensors)/sizeof(Sensor);

  for(uint8_t i = 0; i < size; i++)
  {
    if(sensors[i].start_time) sensors[i].start_time--;

    if(Enable(i) && !sensors[i].start_time)
    {
      switch (sensors[i].type)
      {
        case DOOR:
        case MOVE:
        case RADAR:
          count += sensors[i].get_count();
          break;
        case FIRE:
          count += check_sens_fire(i);
          break;
        case GAS:
        case DHT:
          count += sensors[i].analog_sensor_check();
      }      
    }
  }

  return count;
}
