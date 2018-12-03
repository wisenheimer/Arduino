#ifndef MODEM_H
#define MODEM_H

#include "text.h"
// для работы этой функции выполнить команду AT+CREG=2;&W
#include "beep.h"

#define NLENGTH 14 // Max. length of phone number 
#define TLENGTH 21 // Max. length of text for number
#define DTMF_BUF_SIZE 6

enum common_flag {
  GUARD_ENABLE, // вкл/откл защиту
  ALARM,        // флаг тревоги при срабатывании датчиков
// флаги модема
  RING_ENABLE,  // включает/выключает звонки
  MODEM_NEED_INIT_MODEM
};

#define GET_FLAG(flag)       bitRead(flags,flag)
#define SET_FLAG_ONE(flag)   bitSet(flags,flag)
#define SET_FLAG_ZERO(flag)  bitClear(flags,flag)
#define INVERT_FLAG(flag)    INV_FLAG(flags,1<<flag)

/*
    АТ команды для работы с симкартой
    http://www.comizdat.com/index_.php?in=kpp_articles_id&id=309
*/

typedef struct cell
{
    char    phone[NLENGTH];
    char    name[TLENGTH];
    //char    type[4];
    uint8_t index; // индекс ячейки в телефонной книге  
} ABONENT_CELL;

class MODEM
{
  public:
    uint8_t time_last_answer; // время последнего ответа модема

    MODEM();
    ~MODEM();
    TEXT *email_buffer;
    
    void init();
    void wiring();
    void ring_to_admin();
    void email();

  private:
    TEXT *text;
    void (*func_int)();   
    uint8_t *sens_flag;
    uint8_t answer_flags;
    uint32_t timeRegularOpros;
    uint8_t DTMF[2];    
    uint8_t flag_sensor_enable;
    uint8_t phone_num;
    uint8_t cell_num;
    uint8_t gsm_operator;
    uint8_t reset_count;
    uint8_t gprs_init_count;
    ABONENT_CELL admin; // телефон админа
    char dtmf[DTMF_BUF_SIZE]; // DTMF команда
    uint8_t dtmf_index;
    ABONENT_CELL last_abonent;
    
    void reinit();
    void reinit_end();
    //void ring(uint8_t index);
    bool get_modem_answer(const char* answer, uint16_t wait_ms);
    bool read_com(const char* answer);
    void send_message();
    void parser();
    char* get_number_and_type(char* p);
    char* get_name(char* p);
    void email_send_abonent(ABONENT_CELL abonent);
    void guard_info();
    uint8_t get_sm_cell(uint8_t index);
};

#endif
