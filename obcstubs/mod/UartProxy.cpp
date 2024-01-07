// This is C++ code
#include "UartProxy.h"

#define BYTES_PER_MAINLOOP  100

UartProxy::UartProxy(uart_config *cfg, char *pName, int cdc, Usb *usb) : UartBase(cfg, pName) {
    CdcNr = cdc;
    pUsb = usb;
    StuckByte = -1;
}

 void UartProxy::main() {
    // Base impl - makes TxBuffer to UART work. (Todo: Reads into RxBuffer)
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


 }



//  ------- C API  -------------
UartProxy* createInstanceUartProxy(uart_config *conf, char* pName, int cdc, Usb *usb) {
    return new UartProxy(conf, pName, cdc, usb);
}
