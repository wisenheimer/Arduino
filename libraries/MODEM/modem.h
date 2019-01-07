#ifndef MODEM_H
#define MODEM_H

#include "text.h"
#include "my_sensors.h"

#define NLENGTH 14 // Max. length of phone number 
#define TLENGTH 21 // Max. length of text for number
#define DTMF_BUF_SIZE 6

typedef struct cell
{
    char    phone[NLENGTH];
    char    name[TLENGTH];
    uint8_t index; // индекс ячейки в телефонной книге  
} ABONENT_CELL;

class MODEM
{
  public:
    MODEM::MODEM();
    ~MODEM();
    TEXT *email_buffer;
    
    void init();
    void wiring();
    void ring_to_admin();
    void email();

  private:
    TEXT *text;
    void (*func_int)();   
    uint16_t DTMF[2];
    uint8_t answer_flags;
    uint32_t timeRegularOpros;       
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
    bool get_modem_answer(const char* answer, uint16_t wait_ms);
    bool read_com(const char* answer);
    void parser();
    char* get_number_and_type(char* p);
    char* get_name(char* p);
    void email_send_abonent(ABONENT_CELL abonent);
    void flags_info();
    uint8_t get_sm_cell(uint8_t index);
    void sleep();
};

#endif
