
#include "my_sensors.h"

MY_SENS::MY_SENS()
{

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

void MY_SENS::GetInfoAll(TEXT *buf)
{
  uint8_t size = sizeof(sensors)/sizeof(Sensor);
  for(uint8_t i=0;i<size;i++)
  {
    sensors[i].get_info(buf);
  }
}

void MY_SENS::Clear()
{
	uint8_t size = sizeof(sensors)/sizeof(Sensor);
  for(uint8_t i=0;i<size;i++) sensors[i].count=0;
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

    if(!sensors[i].start_time)
    {
      count += sensors[i].get_count();
    }
  }

#if IR_ENABLE

  IRresume();

#endif 

  return count;
}
