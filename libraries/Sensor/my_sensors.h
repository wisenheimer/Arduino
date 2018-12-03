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

	Sensor sensors[SENS_COUNT] =
	{
		Sensor(DOOR_PIN,DOOR,HIGH,HIGH,0)
#ifdef SENSOR_RADAR_ENABLE
        ,Sensor(RADAR_PIN,RADAR,LOW,LOW)
#endif
#ifdef SENSOR_MOVE_ENABLE
        ,Sensor(MOVE_PIN,MOVE,LOW,LOW)
#endif
#ifdef SENSOR_FIRE_ENABLE
        ,Sensor(FIRE_PIN,FIRE,HIGH,LOW)
#endif
// Инициализируем цифровые датчики с аналоговым выходoм.
// Для экономии пинов ардуино цифровой пин не используем
#ifdef SENSOR_GAS_ENABLE    
        ,Sensor(GAS_PIN,GAS,LOW,LOW,120)
#endif
#ifdef SENSOR_DHT_ENABLE
        ,Sensor(DHT_PIN,DHT)
#endif
    };

    uint8_t check_sens_fire(uint8_t sens_index);
};

#endif // MY_SENS_H