#include "Arduino.h"

enum BeepMode {DEFAULT_BEEP, ALARM_BEEP, NO_BEEP};

void beep_init(uint8_t buzzer_pin);

void beep(uint8_t mode);

void noBeep();