#ifndef myText_h
#define myText_h

//#pragma once /* Защита от двойного подключения заголовочного файла */
#include "Arduino.h"

class TEXT // название класса
{
  public:
    TEXT(uint8_t Size);
    ~TEXT();

    uint8_t index;
    uint8_t free_space;
    // собирает текст для печати
    uint8_t AddText(const char* str);
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
