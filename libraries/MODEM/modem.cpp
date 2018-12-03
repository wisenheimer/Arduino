#include "modem.h"
#include <main_type.h>
#include <EEPROM.h>

uint8_t flags;

enum answer {email_end, smtpsend, smtpsend_end, admin_phone};
#define CLEAR_FLAG_ANSWER           answer_flags=0
#define GET_FLAG_ANSWER(flag)       bitRead(answer_flags,flag)
#define SET_FLAG_ANSWER_ONE(flag)   bitSet(answer_flags,flag)
#define SET_FLAG_ANSWER_ZERO(flag)  bitClear(answer_flags,flag)
#define INVERT_FLAG_ANSWER(flag)    INV_FLAG(answer_flags,1<<flag)

#define OK   "OK"
#define ERR  "ERROR"

// Выставляем тайминги (мс)
// время между опросами модема на предмет зависания и неотправленных смс/email
#define REGULAR_OPROS_TIME  20000

// адрес ячейки, в котором хранится метка о первом включении модуля.
// При первом включении производится инициализация EEPROM, иначе могут быть глюки.
#define EEPROM_FIRST_SMS_ADDRESS    0x00
  #define EEPROM_FIRST_ON_FLAG      0xAA
// ****************************************
// адрес первой ячейки смс в EEPROM
#define EEPROM_SMS_ADDRESS          0x01
// ****************************************
#define EEPROM_SIZE   1024
#define SMS_MAX_SIZE  160 // Максимальная длина смс, символов

#define SERIAL_PRINT    Serial.print
#define SERIAL_PRINTLN  Serial.println

#define ADMIN_PHONE_SET_ZERO  memset(&admin,0,sizeof(ABONENT_CELL))

#define PGM_TO_CHAR(str) strcpy_P(text->GetText()+text->index+1,(char*)pgm_read_word(&(str))) 
// Опрашивает модем
#define READ_COM_FIND_RAM(str) strstr(text->GetText(),str)
#define READ_COM_FIND(str)     strstr_P(text->GetText(),PSTR(str))
#define READ_COM_FIND_P(str)   strstr_P(text->GetText(),(char*)pgm_read_word(&(str)))

#define POWER_ON  digitalWrite(BOOT_PIN,LOW);DELAY(1000);digitalWrite(BOOT_PIN,HIGH)

#define WRITE_SMS_TO_EEPROM   {uint16_t size=text->index;if(size>=SMS_MAX_SIZE-EEPROM_SMS_ADDRESS)size=SMS_MAX_SIZE-EEPROM_SMS_ADDRESS-1; \
                              EEPROM.update(EEPROM_SMS_ADDRESS+size,0x00);while(--size!=255){EEPROM.update(EEPROM_SMS_ADDRESS+size,*(text->GetText()+size));}}

#define GET_MODEM_ANSWER(answer,wait_ms) get_modem_answer(PSTR(answer),wait_ms)

#define GPRS_CONNECT(op)  {SERIAL_PRINT(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\";+SAPBR=3,1,\"APN\",\"internet\";+SAPBR=3,1,\"USER\",\"")); \
                          SERIAL_PRINT(PGM_TO_CHAR(op.user));SERIAL_PRINT(F("\";+SAPBR=3,1,\"PWD\",\""));SERIAL_PRINT(PGM_TO_CHAR(op.user));SERIAL_PRINTLN(F("\";+SAPBR=1,1"));}


#define GPRS_DISCONNECT   SERIAL_PRINTLN(F("AT+SAPBR=0,1"))
#define GPRS_GET_IP       SERIAL_PRINTLN(F("AT+SAPBR=2,1"))

enum {ADMIN};
const char cell_name_0[] PROGMEM = "\"ADMIN\"";
const char* const abonent_name[] PROGMEM = {cell_name_0};

//Эти сообщения пересылаются на e-mail
const char str_0[] PROGMEM = "+CUSD:";   
const char str_1[] PROGMEM = "+CBC:";
const char str_2[] PROGMEM = "ALARM";
const char* const string[] PROGMEM = {str_0, str_1, str_2};

uint8_t op_count; // число операторов 

typedef struct PROGMEM
{
  const char* op;
  const char* user;
} OPERATORS; 

#define ADD_OP(index,name,user) const char op_##index[]    PROGMEM=name; \
                                const char user_##index[]  PROGMEM=user

ADD_OP(0, "MTS",     "mts");
ADD_OP(1, "MEGAFON", "gdata");
ADD_OP(2, "Tele2",   "");
ADD_OP(3, "MOTIV",   "");
ADD_OP(4, "Beeline", "beeline");

const OPERATORS op_base[] PROGMEM = {
  {op_0, user_0},
  {op_1, user_1},
  {op_2, user_2},
  {op_3, user_3},
  {op_4, user_4}
};

// Условия для рестарта модема
#define ADD_RESET(index,name) const char reset_##index[] PROGMEM=name

ADD_RESET(0,NOT_READY);
ADD_RESET(1,"SIM800");
ADD_RESET(2,"NOT READY");
ADD_RESET(3,"SIM not");

const char* const reset[] PROGMEM = {reset_0, reset_1, reset_2, reset_3};

// Условия для завершения звонка
#define ADD_BREAK(index,name) const char ath_##index[] PROGMEM=name

ADD_BREAK(0,"CONNECT");
ADD_BREAK(1,"ANSWER");
ADD_BREAK(2,"BUSY");

const char* const ath[] PROGMEM = {ath_0, ath_1, ath_2};

#ifdef BEEP_ENABLE

void beep()
{
  tone(BEEP_PIN,5000);
  DELAY(1000);
  noTone(BEEP_PIN);
}

#define BEEP_INIT   pinMode(BEEP_PIN,OUTPUT);digitalWrite(BEEP_PIN,LOW)
#define BEEP        beep()

#else

#define BEEP_INIT
#define BEEP

#endif

#ifndef __AVR_ATmega168__
/*
  О прерываниях подробно https://arduinonsk.ru/blog/87-all-pins-interrupts
*/
#include "LowPower.h"
void wakeUp(){}

#ifdef __cplusplus
extern "C"{
  ISR(PCINT2_vect)
  {

  }
}
#endif

#endif

void MODEM::send_message()
{
  int16_t i = EEPROM_SMS_ADDRESS;

  if(EEPROM.read(i))
  {    
#ifndef DEBUG_MODE    
    // отправка смс
    // "AT+CMGS=\"+79xxxxxxxxx\""
    SERIAL_PRINT(F("AT+CMGS=\""));
    SERIAL_PRINT(admin.phone);
    SERIAL_PRINTLN('\"');
    if(!GET_MODEM_ANSWER(">",500)) return;
#endif
    while(EEPROM.read(i) && i < SMS_MAX_SIZE)
    {
      SERIAL_PRINT(EEPROM.read(i++));     
    }
#ifndef DEBUG_MODE
    Serial.flush(); 
    Serial.write(26);
#endif    
  }   
}

MODEM::MODEM()
{
  op_count = sizeof(op_base)/sizeof(OPERATORS);
  gsm_operator = op_count;
  // зуммер
  BEEP_INIT;
  // Устанавливаем скорость связи Ардуино и модема
  Serial.begin(SERIAL_RATE);
}

MODEM::~MODEM()
{
  delete text;
  delete email_buffer;
}

void MODEM::reinit()
{
#ifdef DEBUG_MODE
  SERIAL_PRINTLN(F("BOOT"));
#endif

  POWER_ON; // Включение GSM модуля

  SET_FLAG_ONE(MODEM_NEED_INIT_MODEM);

  reset_count = 0;
  gprs_init_count = 0;
  CLEAR_FLAG_ANSWER;
  timeRegularOpros = millis();
}

void MODEM::init()
{
  phone_num    = 0;
  cell_num     = 0;
  dtmf_index   = 0;

  DTMF[0]      = MODEM_RESET;
  DTMF[1]      = 0;

  ADMIN_PHONE_SET_ZERO;

  text         = new TEXT(TEXT_BUFFER_SIZE);
  email_buffer = new TEXT(EMAIL_BUFFER_SIZE);

  SET_FLAG_ZERO(GUARD_ENABLE);
  SET_FLAG_ZERO(ALARM);
}

char* MODEM::get_number_and_type(char* p)
{
  uint8_t i = 0;

  while((*p!='\"' || *p=='+') && i < NLENGTH-1) last_abonent.phone[i++] = *p++;
        
  last_abonent.phone[i] = 0;

  return p+6;
}

char* MODEM::get_name(char* p)
{
  uint8_t i = 0;
  uint8_t count = 2;

  memset(last_abonent.name, 0, TLENGTH);

  while(count && *p)
  {
    if(*p == '\"') count--;
    last_abonent.name[i++] = *p++;
  }

  return p;
}

void MODEM::parser()
{
  char *p, *pp;
  char ch;

  if(READ_COM_FIND(OK)!=NULL)
  {
    if(GET_FLAG_ANSWER(email_end))
    {
      SET_FLAG_ANSWER_ZERO(email_end);
      SERIAL_PRINT(F("AT+SMTPBODY="));
      SERIAL_PRINTLN(email_buffer->index);      
    }

    if(GET_FLAG_ANSWER(smtpsend))
    {
      SET_FLAG_ANSWER_ZERO(smtpsend);
      SET_FLAG_ANSWER_ONE(smtpsend_end);
      SERIAL_PRINTLN(F("AT+SMTPSEND"));
      gprs_init_count = 0;
    }
  }

  // +CPBS: "SM",18,100
  if((p = READ_COM_FIND("CPBS:"))!=NULL)
  {
    p+=11;
    pp=p;
    while(*p!=',' && *p) p++;
    *p=0;
    phone_num = atoi(pp);
    cell_num = atoi(p+1);

    return;    
  }

  // +CPBF: 20,"+79xxxxxxxxx",145,"ADMIN"
  // +CPBR: 1,"+380xxxxxxxxx",129,"Ivanov"
  // +CPBR: 20,"+79xxxxxxxxx",145,"USER"
  // Запоминаем индекс ячейки телефонной книги
  if((p=READ_COM_FIND("CPBR: "))!=NULL || (p=READ_COM_FIND("CPBF: "))!=NULL)
  {
    p+=6; pp = p;
    email_buffer->AddText(p);
    while(*p!=',' && *p) p++;
    *p = 0;
    last_abonent.index = atoi(pp);
    p+=2; pp = p;
    get_name(get_number_and_type(p));

    if(strstr_P(pp,(char*)pgm_read_word(&abonent_name[ADMIN]))!=NULL)
    {
      admin = last_abonent;
      SET_FLAG_ONE(RING_ENABLE);
    }
    return;
  }

  // RING
  //+CLIP: "+79xxxxxxxxx",145,"",0,"USER",0
  //+CLIP: "+79xxxxxxxxx",145,"",0,"",0
  if((p=READ_COM_FIND("CLIP:"))!=NULL)
  {
    email_buffer->AddText(p);
    memset(&last_abonent, 0, sizeof(ABONENT_CELL));

    get_name(get_number_and_type(p+7)+5);
    if(last_abonent.name[2]) // номер зарегистрирован
    {
      if(READ_COM_FIND_RAM(admin.phone)==NULL && admin.phone[0]) // не админ
      {
        if(GET_FLAG(GUARD_ENABLE)) DTMF[0] = GUARD_OFF;
        else DTMF[0] = GUARD_ON;
      }
      else SERIAL_PRINTLN(F("ATA"));          
    }
    else
    {
      SERIAL_PRINTLN(RING_BREAK);
      if(admin.index == 0)
      {
        /* Добавление текущего номера в телефонную книгу */
        strcpy_P(last_abonent.name,(char*)pgm_read_word(&(abonent_name[ADMIN])));    
        SERIAL_PRINT(F("AT+CPBW=,\""));
        SERIAL_PRINT(last_abonent.phone);
        SERIAL_PRINT(F("\",145,"));
        SERIAL_PRINTLN(last_abonent.name);
        DTMF[0] = EMAIL_PHONE_BOOK;       
      }
    } 
    
#ifdef DEBUG_MODE
      SERIAL_PRINT(F("admin.index=")); SERIAL_PRINTLN(admin.index);
      SERIAL_PRINT(F("admin.phone=")); SERIAL_PRINTLN(admin.phone);
      SERIAL_PRINT(F("last_abonent.phone=")); SERIAL_PRINTLN(last_abonent.phone);
#endif    
    return;
  }

  if(gsm_operator == op_count)
  {
    gsm_operator = 0;
    while (gsm_operator < op_count && READ_COM_FIND_P(op_base[gsm_operator].op)==NULL) gsm_operator++;
  } 

  if((p = READ_COM_FIND("DTMF:"))!=NULL) //+DTMF: 2
  { 
    p+=6;
   
    if(*p == '#')
    {
      DTMF[0] = atoi(dtmf);
      dtmf_index=0;
      RING_BREAK;
    }
    else if(*p == '*')
    {
      DTMF[1] = atoi(dtmf);
      dtmf_index=0; 
    }
    else
    {
      dtmf[dtmf_index++] = *p;
      dtmf[dtmf_index] = 0;
      if(dtmf_index > DTMF_BUF_SIZE-2)
      {
        for(uint8_t i = 0; i < dtmf_index; i++)
        {
          dtmf[i] = dtmf[i+1];  
        }      
        dtmf_index--;
      }      
    }
   
    return;    
  }
/*
  if (READ_COM_FIND("CMGS:")!=NULL) // +CMGS: <mr> - индекс отправленного сообщения 
  {
    // удаляем все отправленные сообщения
    SERIAL_PRINTLN(DEL_ALL_READED_SENT_AND_NOTSENT_SMS);

    EEPROM.update(EEPROM_SMS_ADDRESS, 0x00); // смс успешно отправлено
    return;
  }
*/  
  if ((p=READ_COM_FIND("CMTI:"))!=NULL) // получили номер смс в строке вида +CMTI: "SM",0
  {       
    p+=11;
    // получили номер смс в строке вида +CMTI: "SM",0
    // чтение смс из памяти модуля
    SERIAL_PRINT(F("AT+CMGR=")); SERIAL_PRINTLN(p);
    return;
  }

  // +CMGL: 1,"REC UNREAD","679","","18/10/22,19:16:57+12"
  if ((p=READ_COM_FIND("CMGL:"))!=NULL) // получили номер смс в строке
  {       
    // находим индекс смс в памяти модема
    p+=6;
   
    SERIAL_PRINT(F("AT+CMGR="));
    while(*p!=',')
    {  // чтение смс из памяти модуля
       SERIAL_PRINT(*p++);
    }
    SERIAL_PRINTLN();
    return;    
  }

  if (admin.phone[0])
  {
    if (READ_COM_FIND_RAM(admin.phone))
    {
      SET_FLAG_ANSWER_ONE(admin_phone);
      return;
    }      
  }

  // получаем смс
  // Выполнение любой AT+ команды
  if ((p=READ_COM_FIND("AT+"))!=NULL)
  {
    if (GET_FLAG_ANSWER(admin_phone))
    {
      SERIAL_PRINTLN(p);
      SET_FLAG_ANSWER_ZERO(admin_phone);
    }   
    
    return;      
  } 

  for(uint8_t i = 0; i < 4; i++)
  {
    if(READ_COM_FIND_P(reset[i])!=NULL)
    {
      DTMF[0]=MODEM_RESET;
      return;
    }
  }

  for(uint8_t i = 0; i < 3; i++)
  {
    if(READ_COM_FIND_P(ath[i])!=NULL)
    {
      RING_BREAK;
      return;
    }
  }

  if(READ_COM_FIND("DIALTON")!=NULL)
  {
    ring_to_admin();
    return;    
  }

  for(uint8_t i = 0; i < 3; i++)
  {
    if((p=READ_COM_FIND_P(string[i]))!=NULL)
    {
      email_buffer->AddText(p);
      return;
    }
  }

////////////////////////////////////////////////////////////
/// Для GPRS
////////////////////////////////////////////////////////////
  // требуется загрузка текста письма для отправки
  if(READ_COM_FIND("DOWNLOAD")!=NULL)
  {
    SERIAL_PRINT(email_buffer->GetText());
    Serial.flush();    
    Serial.write(26);
    SET_FLAG_ANSWER_ONE(smtpsend);
    return;        
  }

  if((p = READ_COM_FIND("+SMTPSEND:"))!=NULL)
  {
    SET_FLAG_ANSWER_ZERO(smtpsend_end); 

    // письмо успешно отправлено
    if((p = READ_COM_FIND(": 1"))!=NULL)
    {
      email_buffer->Clear();      

#ifndef __AVR_ATmega168__
      if(!digitalRead(POWER_PIN) && !GET_FLAG(ALARM))
      {
        // Маскируем прерывания
        PCMSK2 |= 0b11110000;
        PCMSK0 = 0xFF;
  
        // разрешаем прерывания
        PCICR |= bit(2);
        // Прерывание для RING
        attachInterrupt(0, wakeUp, LOW); //Если на 0-вом прерываниии - ноль, то просыпаемся.

        GPRS_DISCONNECT; // разрываем соединение с интернетом
        
        SERIAL_PRINTLN(F("AT+CSCLK=2")); // Режим энергосбережения
        
        Serial.flush();
        
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
        // запрещаем прерывания
        PCICR &= ~bit(2);
        detachInterrupt(0);

        do
        {
          text->Clear();
          SERIAL_PRINTLN(F("AT+CSCLK=0")); // Выкл. режим энергосбережения
          DELAY(500);
          SERIAL_PRINTLN(F("AT+CSCLK?"));
        }
        while(!GET_MODEM_ANSWER("CSCLK: 0", 1000));
                
        email_buffer->AddText_P(PSTR("WakeUp:"));    
      }
#endif 
       
    }
    return;    
  }

  if((p = READ_COM_FIND("+SAPBR"))!=NULL)
  {
    if((p = READ_COM_FIND(": 1,1"))!=NULL)
    {
      email();  
    }
    else GPRS_CONNECT(op_base[gsm_operator])
    return;
  }   
//////////////////////////////////////////////////////////// 
}

void MODEM::reinit_end()
{
  if(GET_FLAG(MODEM_NEED_INIT_MODEM))
  {
    const __FlashStringHelper* cmd[] = {
      F("AT+DDET=1"),         // вкл. DTMF. 
      //F("ATS0=0"),            // устанавливает количество гудков до автоответа
      F("AT+CLTS=1"),         // синхронизация времени по сети
      F("ATE0"),              // выключаем эхо
      F("AT+CLIP=1"),         // Включаем АОН
      F("AT+CMGF=1"),         // Формат СМС = ASCII текст
      F("AT+IFC=1,1"),        // устанавливает программный контроль потоком передачи данных
      F("AT+CSCS=\"GSM\""),   // Режим кодировки текста = GSM (только англ.)
      F("AT+CNMI=2,2,0,0,0"), // Текст смс выводится в com-порт
      //F("AT+CSCB=1"),       // Отключаем рассылку широковещательных сообщений        
      //F("AT+CNMI=1,1")      // команда включает отображение номера пришедшей СМСки +CMTI: «MT», <номер смски>
      F("AT+CNMI=2,2"),       // выводит сразу в сериал порт, не сохраняя в симкарте
      F("AT+CPBS=\"SM\""),    // вкл. доступ к sim карте
      DEL_ALL_SMS,
      SIM_RECORDS_INFO        // определить количество занятых записей и их общее число
    };

    uint8_t size = sizeof(cmd)/sizeof(cmd[0]);

    for(uint8_t i = 0; i < size; i++)
    {
      SERIAL_PRINTLN(cmd[i]);
      if(!GET_MODEM_ANSWER(OK, 10000)) return;
    }   
/*
    if(EEPROM.read(EEPROM_FIRST_SMS_ADDRESS) != EEPROM_FIRST_ON_FLAG)
    {
      EEPROM.update(EEPROM_FIRST_SMS_ADDRESS, EEPROM_FIRST_ON_FLAG);
      EEPROM.update(EEPROM_SMS_ADDRESS,0x00);
      SERIAL_PRINTLN(F("AT+CALA=\"09:00:00\",1,0;+CALA=\"17:00:00\",2,0"));
    }
*/
    if(admin.index == 0)
    {
      DTMF[0] = EMAIL_ADMIN_PHONE;        
    }    

#ifdef DEBUG_MODE
    SERIAL_PRINTLN(F("init end"));
#endif
    SET_FLAG_ZERO(MODEM_NEED_INIT_MODEM);  
  }     
}

void MODEM::guard_info()
{
  email_buffer->AddText_P(PSTR("GUARD="));
  email_buffer->AddInt(GET_FLAG(GUARD_ENABLE));
  email_buffer->AddChar('\n');
}

#define DELETE_PHONE(index)  SERIAL_PRINT(F("AT+CPBW="));SERIAL_PRINTLN(index)

#define ON_OFF_FLAG_STATUS(flag,name) email_buffer->AddText_P(PSTR(name));email_buffer->AddInt(GET_FLAG(flag))
#define ON_OFF_FLAG(flag,name)        INVERT_FLAG(flag);ON_OFF_FLAG_STATUS(flag,name)

void MODEM::wiring() // прослушиваем телефон
{
  uint8_t tmp;

  read_com(NULL);

  if(DTMF[0])
  {
    BEEP;

    switch (DTMF[0])
    {
      case GUARD_ON:
        SET_FLAG_ONE(GUARD_ENABLE);
        guard_info();
        break;                        
      case GUARD_OFF:
        SET_FLAG_ZERO(GUARD_ENABLE);       
        guard_info();
        break;        
      case GUARD_INFO:
        guard_info();
        break;
      case TEL_ON_OFF:
        ON_OFF_FLAG(RING_ENABLE, "TEL=");        
        break;
      case BAT_CHARGE:
        SERIAL_PRINTLN(F("AT+CBC"));
        break;
      case NUMBER_DEL_BY_INDEX:
        DELETE_PHONE(DTMF[1]);
        break;
      case SENS_ON_OFF: // установка flag_enable для датчиков
        email_buffer->AddText_P(PSTR("flag="));
        email_buffer->AddInt(*sens_flag); 
        if(DTMF[1]%10)
        bitSet(*sens_flag,DTMF[1]/10); //21/10=2
        else bitClear(*sens_flag,DTMF[1]/10); //20/10=2        
        email_buffer->AddText_P(PSTR("flag="));
        email_buffer->AddInt(*sens_flag);          
        break;
      case EMAIL_ADMIN_PHONE: // получение номера админа на email
        SERIAL_PRINT(F("AT+CPBF="));
        SERIAL_PRINTLN(PGM_TO_CHAR(abonent_name[ADMIN]));        
        break;
      case EMAIL_PHONE_BOOK: // получение на email всех записей на симкарте
        SERIAL_PRINTLN(F("AT+CPBF"));
        break;
      case ADMIN_NUMBER_DEL:
        DELETE_PHONE(admin.index);
        ADMIN_PHONE_SET_ZERO;
        break;
      case SM_CLEAR:
        SERIAL_PRINTLN(SIM_RECORDS_INFO);
        if(GET_MODEM_ANSWER(OK, 5000))
        {
          ADMIN_PHONE_SET_ZERO;
          tmp = 0;
          uint8_t index = 1;
          while(tmp < phone_num && index <= cell_num)
          {
            SERIAL_PRINT(F("AT+CPBR=")); SERIAL_PRINTLN(index);
            if(GET_MODEM_ANSWER("CPBR:", 5000))
            {
              tmp++;
              SERIAL_PRINT(F("AT+CPBW=")); SERIAL_PRINTLN(index);
            }
            index++;
          }  
          SERIAL_PRINTLN(SIM_RECORDS_INFO);
        }
        BEEP;
        break;
      case MODEM_RESET:
        reinit();
        break;
     
      default:
      {
        SERIAL_PRINT(F("AT+CUSD=1,\"#"));
        if(DTMF[1])
        {
          SERIAL_PRINT(DTMF[1]); SERIAL_PRINT('*');
        }
        SERIAL_PRINT(DTMF[0]); SERIAL_PRINTLN(F("#\""));        
      }           
    }
    DTMF[0] = 0;
    DTMF[1] = 0;
    dtmf_index = 0;
  }

  // Опрос модема
  if(millis() - timeRegularOpros > REGULAR_OPROS_TIME)
  {
    SET_FLAG_ANSWER_ZERO(admin_phone);

    reset_count++;
    SERIAL_PRINTLN(F("AT"));
    if(reset_count > RESET_COUNT  || gprs_init_count > 3) 
    {
      DTMF[0]=MODEM_RESET;
    }
   
    reinit_end();

    // Есть данные для отправки
    if(email_buffer->index)
    {
      if(gsm_operator == op_count)
      {
        SERIAL_PRINTLN(F("AT+COPS?"));
      }
      else
      {
        if(!GET_FLAG_ANSWER(smtpsend_end))
        {// проверяем подключение к интернету
          GPRS_GET_IP; // устанавливаем соединение с интернетом
          gprs_init_count++;
        }
      }      
    }
    // проверяем непрочитанные смс
    //SERIAL_PRINTLN(REC_UNREAD_SMS);
    //SERIAL_PRINTLN(MODEM_GET_SIGNAL);
    timeRegularOpros = millis();       
  }
}

// обзвон всех номеров
void MODEM::ring_to_admin()
{
  if(GET_FLAG(RING_ENABLE))
  {
    // позвонить по номеру из сим карты
    SERIAL_PRINT(F("ATD>")); SERIAL_PRINT(admin.index); SERIAL_PRINTLN(';');
  }
}

void MODEM::email()
{
  if(email_buffer->index)
  {
    uint8_t i, size;

    const __FlashStringHelper* cmd[] = 
    {
      F("AT+EMAILCID=1;+EMAILTO=30;+SMTPSRV="), SMTP_SERVER,
      F(";+SMTPAUTH=1,"), SMTP_USER_NAME_AND_PASSWORD,
      F(";+SMTPFROM="), SENDER_ADDRESS_AND_NAME,
      F(";+SMTPSUB=\"Info\""),
      F(";+SMTPRCPT=0,0,"), RCPT_ADDRESS_AND_NAME
#ifdef RCPT_CC_ADDRESS_AND_NAME
      ,F(";+SMTPRCPT=1,0,"), RCPT_CC_ADDRESS_AND_NAME // копия
#ifdef RCPT_BB_ADDRESS_AND_NAME
      ,F(";+SMTPRCPT=2,0,"), RCPT_BB_ADDRESS_AND_NAME // вторая копия
#endif
#endif
    };    

    size = sizeof(cmd)/sizeof(cmd[0]);
  
    for(i = 0; i < size; i++)
    {
      SERIAL_PRINT(cmd[i]);
      Serial.flush();
    } 

    SERIAL_PRINTLN();

    SET_FLAG_ANSWER_ONE(email_end);
  }
}

void MODEM::email_send_abonent(ABONENT_CELL abonent)
{
  email_buffer->AddText_P(PSTR("\nindex:"));email_buffer->AddInt(abonent.index);
  email_buffer->AddText_P(PSTR("\nphone:"));email_buffer->AddText(abonent.phone);
  email_buffer->AddText_P(PSTR("\nname:"));email_buffer->AddText(abonent.name);
  email_buffer->AddChar('\n');
}

bool MODEM::read_com(const char* answer)
{
  char inChar;

  while (Serial.available()) {
    // получаем новый байт:
    inChar = (char)Serial.read();
    // добавляем его:
    if(!text->AddChar(inChar))
    {
      text->Clear();
    }    
    // если получили символ новой строки, оповещаем программу об этом,
    // чтобы она могла принять дальнейшие действия.
    if (inChar == '\n') {
#ifdef DEBUG_MODE
      SERIAL_PRINT(text->GetText());
#endif
      if(text->index > 1)
      {
        reset_count = 0;
        timeRegularOpros = millis();
        if(answer!=NULL)
        {
          if(strstr_P(text->GetText(),answer)!=NULL)
          {
            text->Clear();
            return true;
          }
        }            
        parser();        
      }
      text->Clear();      
    }
  }
  return false;
}

bool MODEM::get_modem_answer(const char* answer, uint16_t wait_ms)
{
  bool res;
  uint32_t time = millis() + wait_ms;

  do
  {    
    res = read_com(answer);        
  }
  while(millis() < time && !res);

  return res;
}