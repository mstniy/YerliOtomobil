#include "LPC407x_8x_177x_8x.h"

#include "Library/UART.h"

void init() {
	uart_init(0, 115200);
	uart_init(3, 9600);
}

char buf[128];
int i;
volatile int x;

void update_bluetooth() {
	uint32_t n_read;
	
	n_read = uart_readline(0, "\r", buf);
	buf[n_read++] = '\n';
	buf[n_read] = '\0';
	uart_write(3, buf); // Send to bluetooth
}

void init_hm10() {
	uart_write(3, "AT+NAMEYerliOtomobil\r\n");
	uart_write(3, "AT+RESET\r\n");
}

void serial_recv_callback(volatile char* buffer, int old_size, int new_size) {
	volatile char* cur;
	for (cur = buffer+old_size; cur<buffer+new_size; cur++) {
		uart_write_n(0, (const char*)cur, 1);
		if (*cur == '\r')
			uart_write(0, "\n");
	}
}

void hm10_recv_callback(volatile char* buffer, int old_size, int new_size) {
	uart_write_n(0, (const char *)(buffer+old_size), new_size-old_size);
	uart_clear_buffer(3); // We don't explicitly read from hm10, so to keep the bufer from filling up, we clean it here.
												// Note that once we implement the logic that reads commands from HM10, we will need to remove this line.
}
 
int main() {
	char* s = "hello world\r\n";

  init();
	uart_write(0, s);
	
	uart_attach_recv_callback(0, serial_recv_callback);
	uart_attach_recv_callback(3, hm10_recv_callback);
	
	init_hm10();

	while (1) {
		update_bluetooth();
	}
}
