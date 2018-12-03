#ifndef SENS_h
#define SENS_h

#include "Arduino.h"
#include "text.h"
#include "settings.h"

#ifdef SENSOR_DHT_ENABLE

  #include <DHT_11.h>

#endif

enum {DOOR,MOVE,RADAR,GAS,FIRE,DHT};

class Sensor   // название класса
{
  public:

    // volatile означает указание компилятору не оптимизировать код её чтения,
    // поскольку её значение может изменяться внутри обработчика прерывания
    volatile uint8_t start_time;
    volatile uint8_t end_time;
    uint8_t count; // количество срабатываний
    uint8_t type;  // тип датчика
    uint8_t pin;   // цифрововй вывод
    bool level;    // высокий или низкий уровень пина
   
    // только цифровой пин
    Sensor::Sensor(uint8_t dpin, uint8_t dtype, uint8_t start_time_sec = 10);
    Sensor(uint8_t dpin, uint8_t dtype, uint8_t pinLevel, uint8_t pin_init_state, uint8_t start_time_sec = 10);
    ~Sensor();
    
    uint16_t get_data();    // возвращает analogRead(data_pin)
    bool get_pin_state();       // возвращает state = digitalRead(pin). Если состояние изменилось, увеличивет счётчик срабатываний на 1.
    uint8_t get_count();        // возвращает счётчик срабатываний датчика.
    void get_info(TEXT *str);   // возвращает строку с именем датчика и числом срабатываний
    void get_name_for_type(TEXT *str);
    bool analog_sensor_check();
    
  private:
  
    uint8_t data_pin;       // аналоговый вывод
    uint8_t step;           // шаг показаний для аналоговых датчиков
    bool prev_pin_state;    // предыдущее состояние пина    

#ifdef SENSOR_DHT_ENABLE
    dht11* dht; // датчик температуры и влажности   
#endif
};

#endif

