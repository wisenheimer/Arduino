#ifndef SENS_h
#define SENS_h

#include "Arduino.h"
#include "settings.h"
#include "text.h"

#if IR_ENABLE
#   include "ir.h"
#endif

#if DHT_ENABLE
#   include "stDHT.h"
#endif

// Сюда добавляем типы датчиков
enum {  // датчик с одним цифровым выходом
        DIGITAL_SENSOR,
        // датчик с одним цифровым выходом,
        // с проверкой от ложного срабатывания
        CHECK_DIGITAL_SENSOR,
        // датчик с аналоговым выходом
        ANALOG_SENSOR,
        // беспроводной датчик с ик диодом
        IR_SENSOR,
        // датчик температуры - термистор
        TERMISTOR          
        //DHT11, DHT21, DHT22 - датчики температуры
        // и влажности, объявлены в stDHT.h
};

class Sensor
{
  public:

    // volatile означает указание компилятору не оптимизировать код её чтения,
    // поскольку её значение может изменяться внутри обработчика прерывания
    volatile uint8_t start_time;
    volatile uint8_t end_time;
    uint8_t count; // количество срабатываний
    uint8_t type;  // тип датчика
    uint8_t pin;   // пин
    bool level;    // высокий или низкий уровень пина
    bool prev_pin_state; // предыдущее состояние пина
   
    Sensor(uint8_t _type, char* sens_name, uint32_t ir_code);
    Sensor(uint8_t _pin, uint8_t _type, char* sens_name, uint8_t pinLevel = LOW, uint8_t start_time_sec = 10, uint32_t alarm_val = 200);
    ~Sensor();
    
    bool get_pin_state();       // возвращает state = digitalRead(pin). Если состояние изменилось, увеличивет счётчик срабатываний на 1.
    uint8_t get_count();        // возвращает счётчик срабатываний датчика.
    void get_info(TEXT *str);   // возвращает строку с именем датчика и числом срабатываний
    void get_name_for_type(TEXT *str);  
    
  private:
    bool check;
    int32_t alarm_value; // значение срабатывания аналогового датчика
    char *name;
#if DHT_ENABLE
    DHT* dht; // датчик температуры и влажности   
#endif
};

#endif

