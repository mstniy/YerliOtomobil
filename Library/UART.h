/**
 * Blocking UART interface
 * supports UART 0, 2 and 3
 */

#ifndef UART_H
#define UART_H

#include "LPC407x_8x_177x_8x.h"

void uart_init(uint8_t uart_id, uint32_t baud_rate);
uint32_t uart_readline(uint8_t uart_id, const char* eol, char* s);
void uart_write(uint8_t uart_id, const char *s);

#endif
