#ifndef __UART_CHANNEL_H
#define __UART_CHANNEL_H

typedef int (*uart_cb_t)(uint8_t*data, uint32_t len);

void hpm_uart_channel_init(void);

void hpm_uart_channel_handle(uart_cb_t uart_cb);

void hpm_uart_block_task(void* proc);

int hpm_uart_channel_output(uint8_t *data, uint16_t length);

#endif //__UART_CHANNEL_H