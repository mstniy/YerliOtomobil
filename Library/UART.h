/**
 * Non-blocking UART interface
 * supports UART 0, 2 and 3
 */

#ifndef UART_H
#define UART_H

#include "LPC407x_8x_177x_8x.h"

// Note that we don't directly expose UART buffers, because this would create the illusion that they are safe to access and use directly. They are not.
// They might be changed by a UART interrupt at any moment. Attached recv callbacks can, however, safely access and modify these buffers, because
//   they are called while the interrupt is running.
typedef void (*UARTRecvCallback)(volatile char* buffer, int old_len, int new_len);

void uart_init(uint8_t uart_id, uint32_t baud_rate);
//Returns -1 if the line is not ready yet.
int32_t uart_readline(uint8_t uart_id, const char* eol, char* s);
// uart_write* will flush the send buffer if it gets full.
void uart_write(uint8_t uart_id, const char *s, int blocking);
void uart_write_n(uint8_t uart_id, const char *s, uint32_t len, int blocking);
void uart_attach_recv_callback(uint8_t uart_id, UARTRecvCallback cb);
void uart_clear_recv_buffer(uint8_t uart_id);

#endif
