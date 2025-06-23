#ifndef __CLI_H__
#define	__CLI_H__

#define	CLI_BUF_SIZE		96	
#define	CLI_HISTORY_SIZE	4
#define	CONSOLE_RX_BUF_SIZE	96	
#define	CONSOLE_RX_DELAY_US	10000

extern volatile uint32_t console_rx_buf_len;
extern volatile uint8_t console_rx_buf[];
extern volatile uint32_t console_rx_timestamp;

void cli_prompt(void);
void cli_process_input(uint8_t *buf, uint32_t len);
void console_poll(void);
void console_rx(void);

#endif

