#ifndef DHT11_h
#define DHT11_h

#include "stDHT.h" // https://istarik.ru/blog/arduino/35.html
#include "text.h"

class dht11   // название класса
{
  public:

    dht11(byte dpin);    
    void getInfo(TEXT* str);
    uint8_t GetData();
  
  private:
    uint8_t pin; // цифрововй вывод
    DHT *sens;    
};

#endif
