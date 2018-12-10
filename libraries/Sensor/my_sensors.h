#ifndef MY_SENS_H
#define MY_SENS_H

#include <main_type.h>
//#include "settings.h"
#include "sensor.h"
#include "text.h"

class MY_SENS
{
  public:
    uint8_t flag_enable, tmp;

    MY_SENS();
    ~MY_SENS();
    void GetInfo(TEXT *buf);
    void Clear();
    bool ReadPin(uint8_t sens_index);
    uint8_t Count(uint8_t sens_index);
    void SetOne(uint8_t sens_index);
    void SetZero(uint8_t sens_index);
    bool Enable(uint8_t sens_index);
    void SaveEnableTmp();
    void RestoreEnable();

    uint8_t SensOpros();

    void TimeReset();

  private:
  	// Инициализация датчиков
	SENSORS_INIT
    
    uint8_t get_check_count(uint8_t sens_index);
};

#endif // MY_SENS_H
