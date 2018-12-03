#ifndef DHT_H
#define DHT_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define DHT11 11
#define DHT22 22
#define DHT21 21

class DHT {
  public:
   DHT(uint8_t type, uint8_t count=6);
   void begin(void);
   int readTemperature(int PINS);
   int readHumidity(int PONS);
   boolean read(void);

 private:
  uint8_t data[6];
  uint8_t _pin, _type, _bit, _port;
  uint32_t _lastreadtime, _maxcycles;
  bool _firstreading;
  bool _lastresult;

  uint32_t expectPulse(bool level);

};

class InterruptLock {
  public:
   InterruptLock() {
    noInterrupts();
   }
   ~InterruptLock() {
    interrupts();
   }

};

#endif
