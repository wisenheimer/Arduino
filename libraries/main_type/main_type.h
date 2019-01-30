#ifndef MAIN_TYPE_H
#define MAIN_TYPE_H

#define DELAY _delay_ms

#define SERIAL_PRINT    Serial.print
#define SERIAL_PRINTLN  Serial.println
#define SERIAL_FLUSH	Serial.flush()
#define SERIAL_WRITE	Serial.write

//------------------------------------------------------------------------------
// Debug directives
//
#if DEBUG_MODE
#	define DEBUG_PRINT		Serial.print
#	define DEBUG_PRINTLN	Serial.println
#else
#	define DEBUG_PRINT(...)
#	define DEBUG_PRINTLN(...)
#endif

enum common_flag {
  GUARD_ENABLE, 	// вкл/откл защиту
  GPRS_ENABLE,		// отправка отчётов по e-mail
  CONNECT_ALWAYS,	// не разрывать соединение после отправки e-mail
  SMS_ENABLE,   	// отправка отчётов по sms
  ALARM,        	// флаг тревоги при срабатывании датчиков
  INTERRUPT,    	// прерывание
// флаги модема
  RING_ENABLE,  	// включает/выключает звонки
  MODEM_NEED_INIT
};

#define GET_FLAG(flag)       bitRead(flags,flag)
#define SET_FLAG_ONE(flag)   bitSet(flags,flag)
#define SET_FLAG_ZERO(flag)  bitClear(flags,flag)
#define INVERT_FLAG(flag)    INV_FLAG(flags,1<<flag)

#endif // MAIN_TYPE_H
