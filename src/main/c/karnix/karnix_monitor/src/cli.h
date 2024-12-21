#ifndef __CLI_H__
#define	__CLI_H__

extern uint32_t console_rx_buf_len;
extern uint8_t console_rx_buf[];
extern uint32_t console_rx_timestamp;

void cli_prompt(void);
void cli_process_input(char *buf, int len);
void console_poll(void);
void console_rx(void);

#endif

