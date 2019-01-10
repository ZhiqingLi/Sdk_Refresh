#ifndef __COMMUNICATION_SVR_H__
#define __COMMUNICATION_SVR_H__


#define EQ_NAME_SIZE 12
#define EQ_RX_MAX_SIZE 3072//3072

#define EQ_TX_MAX_SIZE 100

typedef struct{
    int         prefix;
    int         crc;
    char        name[EQ_NAME_SIZE];
	uint32_t    len; 
}eq_head;

typedef struct{
	eq_head head;
	uint8_t rx_buf[EQ_RX_MAX_SIZE];
}eq_msg;


typedef struct{
	eq_head head;
	uint8_t tx_buf[EQ_TX_MAX_SIZE];
}eq_reply;





typedef int (*eq_keywords_callback_t)(uint8_t *buf, uint32_t  len);

#define EQ_KEY_WORD_NUM 6

typedef struct 
{
  char eq_keywords[EQ_NAME_SIZE];
  eq_keywords_callback_t eq_callback;
}eq_keyword_table;



#ifdef __cplusplus
extern "C" {
#endif

void communication_init(void);
void communication_deinit(void);

void eq_reply_payload_updata(unsigned char *buf, unsigned int len);
void uart_audio_init();
void uart_audio_deinit();


#ifdef __cplusplus
}
#endif

#endif

