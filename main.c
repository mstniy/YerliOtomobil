#include "LPC407x_8x_177x_8x.h"

#include "Library/UART.h"

void init() {
	uart_init(0, 115200);
	uart_init(3, 9600);
}

char buf[128];
int i;
volatile int x;

void update_serial_echo() {
	uint32_t n_read;
	
	n_read = uart_readline(0, "\r", buf);
	buf[n_read++] = '\n';
	buf[n_read] = '\0';
	uart_write(0, buf);
}

void update_hm10_terminal() {
	uint32_t n_read;
	
	n_read = uart_readline(0, "\r", buf);
	buf[n_read++] = '\n';
	buf[n_read] = '\0';
	uart_write(0, buf);
	uart_write(3, buf);
	n_read = uart_readline(3, "\r\n", buf);
	uart_write(0, buf);
}

void update_bluetooth_comm() {
	uint32_t n_read;
	
	n_read = uart_readline(0, "\r", buf);
	buf[n_read++] = '\n';
	buf[n_read] = '\0';
	uart_write(0, buf);

	// send to bluetooth
	uart_write(3, buf);

	n_read = uart_readline(3, "\r\n", buf);
	uart_write(0, buf);
}

void init_hm10() {
	uint32_t nread;
	
	/*uart_write(3, "AT\r\n");
	uart_readline(3, "\r\n", buf); // OK
	uart_write(0, buf);*/
	
	uart_write(3, "AT+NAMEYerliOtomobil\r\n");
	nread = uart_readline(3, "\r\n", buf); // 
	uart_readline(3, "\r\n", buf + nread); //
	uart_write(0, buf);
	
	uart_write(3, "AT+RESET\r\n");
	nread = uart_readline(3, "\r\n", buf); // 
	uart_readline(3, "\r\n", buf + nread); //
	uart_write(0, buf);
}
 
int main() {
	char* s = "hello world\r\n";

  init();
	uart_write(0, s);
	
	init_hm10();

	while (1) {
		//update_serial_echo();
		//update_hm10_terminal();
		update_bluetooth_comm();
	}
}
