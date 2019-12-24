/**
 * Blocking UART implementation
 */

#include <string.h>

#include "UART.h"
#include "SystemStructures.h"

static volatile LPC_UART_TypeDef* const UART[] = {
	LPC_UART0, 
	0,
	LPC_UART2,
	LPC_UART3
};

static const uint8_t PCONP_bit[] = {3, 4, 24, 25};

static const uint32_t baud_rate_to_conf[][5] = {
	{115200, 0, 19, 5, 7},
	{9600, 0, 217, 4, 5},
};

static const IRQn_Type UART_IRQn[] = {
	UART0_IRQn, 0, UART2_IRQn, UART3_IRQn
};

static const int UART_RECV_BUFFER_LENGTH = 32;

static volatile char UARTBuffers[4][UART_RECV_BUFFER_LENGTH]; // These are volatile, because they can be changed by an interrupt
static volatile int	 UARTBufferIndexes[4];

static void uart_set_baud_rate(volatile LPC_UART_TypeDef* uart, uint32_t baud_rate) {
	uint8_t i = 0;

	uart->LCR |= (1 << 7); // DLAB = 1

	// search for configuration
	for (i = 0; i < sizeof(baud_rate_to_conf) / (5 * sizeof(uint32_t)); i++)
		if (baud_rate_to_conf[i][0] == baud_rate)
			break;

	// set DLL, DLM, FDR
	uart->DLM = baud_rate_to_conf[i][1];
	uart->DLL = baud_rate_to_conf[i][2];
	uart->FDR = (baud_rate_to_conf[i][3] << 0) | (baud_rate_to_conf[i][4] << 4);
	
	uart->LCR &= ~(1 << 7); // DLAB = 0
}

static void uart_set_pin_function(uint8_t uart_id) {
	if (uart_id == 0) {
		LPC_IOCON->P0_2 |= (LPC_IOCON->P0_2 & 7) | 1;
		LPC_IOCON->P0_3 |= (LPC_IOCON->P0_3 & 7) | 1;
	}
	else if (uart_id == 3) {
		LPC_IOCON->P0_0 |= (LPC_IOCON->P0_0 & 7) | 2;
		LPC_IOCON->P0_1 |= (LPC_IOCON->P0_1 & 7) | 2;
	}
}

void uart_init(uint8_t uart_id, uint32_t baud_rate) {
	uart_set_pin_function(uart_id);

	// Power on UART
	PCONP |= (1 << PCONP_bit[uart_id]);

	// Enable FIFO
	UART[uart_id]->FCR = (1 << 0);

	uart_set_baud_rate(UART[uart_id], baud_rate);

	UART[uart_id]->LCR = (3 << 0) // 8-bit character length
										| (0 << 2) // 1 stop bit
										| (0 << 3); // no parity bit
		
	UART[uart_id]->IER = 1;  // Enable RDA interrupts
	NVIC_EnableIRQ(UART_IRQn[uart_id]);
	NVIC_SetPriority(UART_IRQn[uart_id], 5);
	//NVIC_ClearPendingIRQ(UART_IRQn[uart_id]);
};

void uart_write(uint8_t uart_id, const char *s) {
	for (; *s; s++) {
		// wait until THRE is set
		while (!(UART[uart_id]->LSR & (1 << 5)))
			;
		UART[uart_id]->THR = *s;
	}
}

static void UART_IRQHandler(int uart_id) {
	uint32_t currentInterrupt = UART[uart_id]->IIR;
	if (currentInterrupt & (1<<2)) { // RDA
		while ((UART[uart_id]->LSR & 1) && // Receiver data ready
						UARTBufferIndexes[uart_id] < UART_RECV_BUFFER_LENGTH-1) { // There is space in the buffer
			UARTBuffers[uart_id][UARTBufferIndexes[uart_id]++] = UART[uart_id]->RBR;
		}
	}
}

void UART0_IRQHandler() {
	UART_IRQHandler(0);
}

void UART2_IRQHandler() {
	UART_IRQHandler(2);
}

void UART3_IRQHandler() {
	UART_IRQHandler(3);
}

uint32_t uart_readline(uint8_t uart_id, const char* eol, char *s) {
		uint32_t s_index = 0, i, eol_len, irq_enabled_state;

		eol_len = strlen(eol);

		while(1) {
			while (UARTBufferIndexes[uart_id] == 0)
					;
			
			// Disable interrupts to avoid race conditions
			irq_enabled_state = NVIC_GetEnableIRQ(UART_IRQn[uart_id]);
			NVIC_DisableIRQ(UART_IRQn[uart_id]);
			
			for (i=0; i<UARTBufferIndexes[uart_id]; i++)
				s[s_index++] = UARTBuffers[uart_id][i];
			UARTBufferIndexes[uart_id]=0;
			
			if (irq_enabled_state)
				NVIC_EnableIRQ(UART_IRQn[uart_id]);
			
			// s ends with eol
			if (strncmp(s+s_index-eol_len, eol, eol_len) == 0)
					break;
		}

		s[s_index] = '\0';
		return s_index;
}