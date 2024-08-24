// This is C++ code
#include "UartBase.h"

#define UART_INCREMENT_BUFFERIDX(i) {i++;  if (i >= UART_BUFFER_SIZE) { i = 0; }}

UartBase::UartBase(uart_config *pC, char *pName) : Module(pName) {
    data.cfg = pC;
    data.TxIdx = 0;
    data.WriteIdx = 0;
}

void UartBase::init(void *p) {
	gpio_set_function(data.cfg->txPin, GPIO_FUNC_UART);
	gpio_set_function(data.cfg->rxPin, GPIO_FUNC_UART);

	uart_init(data.cfg->uart, data.cfg->bit_rate);
	uart_set_hw_flow(data.cfg->uart, false, false);
	uart_set_format(data.cfg->uart, 8, 1, UART_PARITY_NONE);
	//uart_set_fifo_enabled(data.cfg->uart, false); 
}


// This callback is used from other core. uart reconfigure seems to work witout mutex here !?
void UartBase::Reconfigure(uint32_t baud, uint8_t bit, uint8_t stop, uint8_t parity ) {
    uart_set_baudrate(data.cfg->uart,  baud);
    //uart_set_format(data.cfg->uart, 8, 1, UART_PARITY_NONE);

    //data.cfg->bit_rate = baud;
    //reconfigure = true;
 }


void UartBase::main() {
    // TX to UART. 
    if (data.TxIdx != data.WriteIdx) {
        // Some bytes still have to be sent. Copy bytes from TX Buffer to UART TXFIFO
        while (uart_is_writable(data.cfg->uart) &&
		       data.TxIdx != data.WriteIdx) {
			uart_putc_raw(data.cfg->uart, data.TxBuffer[data.TxIdx]);
            UART_INCREMENT_BUFFERIDX(data.TxIdx);
		}
    }
    // RX from uart
    // ...
}

bool UartBase::writeByteUart(uint8_t b) {
    // put this byte into Uart Tx buffer
    data.TxBuffer[data.WriteIdx] = b;  
    UART_INCREMENT_BUFFERIDX(data.WriteIdx);
    if (data.WriteIdx == data.TxIdx) {
        // Buffer overrun
        data.WriteIdx--;
        // Count error ...
        return false;
    }
    return true;
}
