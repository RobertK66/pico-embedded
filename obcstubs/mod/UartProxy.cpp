// This is C++ code
#include "UartProxy.h"

#define BYTES_PER_MAINLOOP  100

UartProxy::UartProxy(uart_config *cfg, char *pName, int cdc, Usb *usb) : UartBase(cfg, pName) {
    CdcNr = cdc;
    pUsb = usb;
    StuckByte = -1;
}

 void UartProxy::main() {
    // Base impl - makes TxBuffer to UART work. 
    UartBase::main();

    // receive from USB-COM --> send to UART (put into TxBuffer).
    int cnt = 0;
    int b = StuckByte;
    do {
        if (StuckByte == -1) {
            b = pUsb->readNextRxByte(CdcNr);
        }
        if (b >= 0) {
            if (writeByteUart((uint8_t)b)) {
                StuckByte = -1;
                cnt++;
            } else { 
                // Stop and keep the one byte not fit into buffer yet.
                StuckByte = b;
                b = -1;
            }
        }
    } while ((b>=0) && (cnt < BYTES_PER_MAINLOOP));
    
    // receive from UART --> send to USB COM (buffer).
    // check if RxBuffer contains bytes.
    if (uart_is_readable(data.cfg->uart)) {
		//mutex_enter_blocking(&ud->uart_mtx);
		while (uart_is_readable(data.cfg->uart)) {
			uint8_t b = uart_getc(data.cfg->uart);
            pUsb->writeByteCdc(CdcNr, b);
            //pUsb->flushCdc(CdcNr);
		}
        pUsb->flushCdc(CdcNr);
		//mutex_exit(&ud->uart_mtx);
	}




 }



//  ------- C API  -------------
UartProxy* createInstanceUartProxy(uart_config *conf, char* pName, int cdc, Usb *usb) {
    return new UartProxy(conf, pName, cdc, usb);
}
