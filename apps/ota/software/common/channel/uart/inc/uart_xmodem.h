#ifndef __UART_XMODEM_H
#define __UART_XMODEM_H

#define USE_UART_ID   HPM_UART0

int xmodemReceive(unsigned char* dest, int destsz, void* proc);

#endif// __UART_XMODEM_H