#include "text.h"

TEXT::TEXT(uint8_t Size)
{
  free_space = Size-1;  
  bufSize = Size;
  index = 0;

  Buffer = (char*)calloc(1, Size);
}

TEXT::~TEXT()
{
  free(Buffer);
}

uint8_t TEXT::AddText(char* str)
{
  while(AddChar(*str++));
}

uint8_t TEXT::AddText_P(const char* str)
{
  while(AddChar(pgm_read_byte(str++)));
}

bool TEXT::AddChar(char ch)
{
  if(free_space && ch)
  {
    Buffer[index++] = ch;
    Buffer[index] = 0x00;
    free_space--;
    return true;    
  }
  return false;
}

void TEXT::AddInt(int i)
{
  char *buf = (char*)calloc(1,7);
  AddText(itoa(i,buf,10));
  free(buf);
}

void TEXT::Clear()
{
  free_space = bufSize-1;
  index = 0;
  memset(Buffer, 0x00, bufSize);
}

char* TEXT::GetText()
{
  return Buffer;
}