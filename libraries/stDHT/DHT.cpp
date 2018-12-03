#include "stDHT.h"

DHT::DHT(uint8_t type, uint8_t count) 
 {
   _type = type;	 
   _firstreading = true;
   _lastreadtime = 0;
 }

int DHT::readTemperature(int PINS) 
{
   _pin = PINS;	
   _bit = digitalPinToBitMask(_pin);
   _port = digitalPinToPort(_pin);
   _maxcycles = microsecondsToClockCycles(1000);  
 
  int f = NAN;
  
  if(read()) 
   {
    switch (_type) 
    {
    case DHT11:
      f = data[2];
      break;
      
    case DHT22:
    case DHT21:
      f = data[2] & 0x7F;
      f *= 256;
      f += data[3];
      f /= 10;
      if(data[2] & 0x80) 
       {
        f *= -1;
       }
      break;
     }
   }
   
  return f;
}

int DHT::readHumidity(int PONS) 
 {
   _pin = PONS;	
   _bit = digitalPinToBitMask(_pin);
   _port = digitalPinToPort(_pin);
   _maxcycles = microsecondsToClockCycles(1000); 
   	 
   int f = NAN;
   if(read()) 
    {
     switch (_type) 
      {
       case DHT11:
        f = data[0];
        break;
        
       case DHT22:
       case DHT21:
        f = data[0];
        f *= 256;
        f += data[1];
        f /= 10;
        break;
    }
  }
    
   return f;
 }

boolean DHT::read(void) 
{
  uint32_t currenttime = millis();
  if(currenttime < _lastreadtime) 
    {
      _lastreadtime = 0;
    }
    
  if(!_firstreading && ((currenttime - _lastreadtime) < 2000)) 
    {
      return _lastresult; 
    }
    
  _firstreading = false;
  _lastreadtime = millis();

  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  digitalWrite(_pin, HIGH);
  delay(50);//////////////////////// Изначально это значение было - 250. Если что-то будет работать "криво",
  ////////////////////////////////// попробуйте увеличивать значение с 50-ти до 60 и т.д.

  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(20); 

  uint32_t cycles[80];
  {
    InterruptLock lock;
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);
    pinMode(_pin, INPUT);
    delayMicroseconds(10);  

    if(expectPulse(LOW) == 0) 
     {
      _lastresult = false;
      return _lastresult;
     }
     
    if(expectPulse(HIGH) == 0) 
     {
      _lastresult = false;
      return _lastresult;
     }


    for(int i=0; i<80; i+=2) 
     {
      cycles[i]   = expectPulse(LOW);
      cycles[i+1] = expectPulse(HIGH);
     }
  } 

  for(int i=0; i<40; ++i) 
   {
     uint32_t lowCycles  = cycles[2*i];
     uint32_t highCycles = cycles[2*i+1];
     if((lowCycles == 0) || (highCycles == 0)) 
      {
        _lastresult = false;
        return _lastresult;
      }
    data[i/8] <<= 1;

    if(highCycles > lowCycles) 
     {
       data[i/8] |= 1;
     }
   }


  if(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) 
   {
     _lastresult = true;
     return _lastresult;
   }
   
  else 
   {
     _lastresult = false;
     return _lastresult;
   }
}

uint32_t DHT::expectPulse(bool level) 
 {
    uint32_t count = 0;
    uint8_t portState = level ? _bit : 0;
    while((*portInputRegister(_port) & _bit) == portState) 
     {
      if(count++ >= _maxcycles) 
       {
        return 0;
       }
     }

   return count;
}
