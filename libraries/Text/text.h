#ifndef myText_h
#define myText_h

#include "Arduino.h"

class TEXT
{
  public:
    TEXT(uint8_t Size);
    ~TEXT();

    uint8_t index;
    uint8_t free_space;
    uint8_t AddText(char* str);
    uint8_t AddText_P(const char* str);
    bool AddChar(char ch);
    void AddInt(int i);
    void Clear();
    char* GetText();
    
  private:
    char* Buffer;    
    uint8_t bufSize;
};

#endif
