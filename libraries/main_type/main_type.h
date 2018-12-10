#ifndef MAIN_TYPE_H
#define MAIN_TYPE_H

#include "settings.h"

#define DELAY _delay_ms

#define INV_FLAG(buf,flag)     		(buf^=(flag))	// инвертировать бит

/*
 * Модем
 */
// Чтение смс
#define SIM_RECORDS_INFO F("AT+CPBS?") // определить количество занятых записей и их общее число

#define REC_UNREAD_SMS			"AT+CMGL=\"REC UNREAD\",1"	

#define GET_MODEM_TIME			F("AT+CCLK?")

#define MODEM_GET_STATUS      	F("AT+CPAS")
  #define READY               	"CPAS: 0"
  #define NOT_READY           	"CPAS: 1"
  #define RINGING             	"CPAS: 3"
  #define CALL_IN_PROGRESS    	"CPAS: 4"
  #define ASLEEP              	"CPAS: 5"  

#define MODEM_GET_SIGNAL      	F("AT+CSQ")
  #define NOT_SIGNAL          	" 99"

// зарегистрирован ли в домашней сети сети
#define MODEM_NETWORK_REGISTRED F("AT+CREG?")
  #define REGISTRED           	"CREG: 0,1"

#define DEL_ONE_SMS                         F("AT+CMGD=1,0") // удалить первое сообщение
#define DEL_ALL_READED_SMS                  F("AT+CMGD=1,1") // удалить все прочитанные смс
#define DEL_ALL_READED_AND_SENT_SMS         F("AT+CMGD=1,2") // удалить все прочитанные и отправленные смс
#define DEL_ALL_READED_SENT_AND_NOTSENT_SMS F("AT+CMGD=1,3") // удалить все прочитанные, отправленные и неотправленные смс
#define DEL_ALL_SMS                         F("AT+CMGD=1,4") // удалить все смс

#define RING_BREAK                          SERIAL_PRINTLN(F("ATH")) // разорвать все соединения

#define TEXT_BUFFER_SIZE       100
#define EMAIL_BUFFER_SIZE      255
#define TIME_STRING_SIZE       23

#endif // MAIN_TYPE
