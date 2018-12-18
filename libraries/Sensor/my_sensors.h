#ifndef MY_SENS_H
#define MY_SENS_H

#include "settings.h"
//#include "settings.h"
#include "sensor.h"
#include "text.h"

class MY_SENS
{
  public:
    uint8_t tmp;

    MY_SENS();
    ~MY_SENS();
    void GetInfo(TEXT *buf);
    void Clear();
    uint8_t Count(uint8_t sens_index);
    void SetOne(uint8_t sens_index);
    void SetZero(uint8_t sens_index);
    bool Enable(uint8_t sens_index);
    void SaveEnableTmp();
    void RestoreEnable();

    uint8_t SensOpros();

    void TimeReset();

  private:
    uint8_t flag_enable;
  	// Инициализация датчиков
	SENSORS_INIT
};

#endif // MY_SENS_H
