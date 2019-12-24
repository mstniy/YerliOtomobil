#include <string.h>
#include "HM10.h"

void hm10_init(const char* name) {
	static char buf[128];
	uart_init(3, 9600);
	strcpy(buf, "AT+NAME");
	strcat(buf, name);
	strcat(buf, "\r\n");
	uart_write(3, buf);
	uart_write(3, "AT+RESET\r\n");
}
