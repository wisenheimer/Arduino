
#include "my_sensors.h"

MY_SENS::MY_SENS()
{
	// Флаг flag_enable разрешает датчикам быть источником для включения режима тревоги.
	// Каждый бит соответствует подключенному сенсору, в том порядке, как они перечислены в
	// массиве sensors[] в файле "settings.h".
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
    if(sensors[i].count)
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
      count += sensors[i].get_count();
    }
  }

  return count;
}
