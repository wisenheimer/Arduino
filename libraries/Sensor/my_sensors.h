#ifndef MY_SENS_H
#define MY_SENS_H

#include "settings.h"
#include "sensor.h"
#include "text.h"

class MY_SENS
{
  public:
    uint8_t tmp;

    MY_SENS();
    ~MY_SENS();
    void GetInfo(TEXT *buf);
    void GetInfoAll(TEXT *buf);
    void Clear();
    uint8_t SensOpros();

    void TimeReset();

  private:
  	// Инициализация датчиков
	SENSORS_INIT
};

#endif // MY_SENS_H
