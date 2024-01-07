// This is C++ code
#include "Usb.h"

#include <iostream>
#include <stdlib.h>
#include <tusb.h>

Usb::Usb( ) : Module::Module((char *)"Usb")  { 
    usbdata[0].con = false;
    usbdata[0].RxIdx = 0;
    usbdata[0].ReadIdx = 0;
    usbdata[0].pUart = 0;

    usbdata[1].con = false;
    usbdata[1].RxIdx = 0;
    usbdata[1].ReadIdx = 0;
    usbdata[1].pUart = 0;
}

void Usb::SetUartProxy(UartBase *pCdc1Uart, UartBase *pCdc2Uart) {
     usbdata[0].pUart = pCdc1Uart;
     usbdata[1].pUart = pCdc2Uart;
}

void Usb::init(void *p) {
    //tusb_init(); this has to be done before stdio_init !!!
}

void Usb::main() { 
    tud_task();     // This serves the tiny USB main task to establish CDCs on USB composite device.

    // Check both CDCs fro received data             
    for (int nr=1; nr<=2; nr++) {
        bool c = tud_cdc_n_connected(nr);
        usb_buffer *ub = &usbdata[nr-1];
        if (ub->con != c) {
            ub->con = c;
            if (ub->con) {
                cdc_line_coding_t lc;
                tud_cdc_n_get_line_coding(nr, &lc);
                printf("COM-%i connected with %i,%i,%i,%i\n", nr, lc.bit_rate, lc.data_bits, lc.stop_bits, lc.parity);
                if (ub->pUart != 0) {                      
                    ub->pUart->Reconfigure( lc.bit_rate, lc.data_bits, lc.stop_bits, lc.parity );            
                }
            } else {
                printf("COM-%i disconnected.\n", nr);
            }
        }
        if (ub->con) {
            uint32_t len = tud_cdc_n_available(nr);
            if (len>0) {
                // TODO: check for overrun here and count errors....
                uint32_t len1 = MIN(len, BUFFER_SIZE - ub->RxIdx);
                ub->RxIdx += tud_cdc_n_read(nr, &(ub->RxBuffer[ub->RxIdx]), len1);
                if (len1<len) {
                    // Read the rest and stick it to front of ring buffer array.
                    ub->RxIdx = tud_cdc_n_read(nr, &(ub->RxBuffer[0]), len - len1);
                }
                if (ub->RxIdx >= BUFFER_SIZE) {
                    ub->RxIdx = 0;
                }
                //printf("Rx[%i] %i bytes. Buffered Up to: %i\n", nr, len, ub->RxIdx);
            }
        }
    }
}

void Usb::writeByteCdc(int cdcNr, uint8_t b) {
    assert((cdcNr>=1) && (cdcNr<=2));         // exactly 2 CDC nr (1,2) are valid!
    usb_buffer *ub = &usbdata[cdcNr-1];
    if (ub->con) {
        tud_cdc_n_write_char(cdcNr, b);
        //tud_cdc_n_write_flush(cdcNr);
    }
}

void Usb::flushCdc(int cdcNr) {
    assert((cdcNr>=1) && (cdcNr<=2));         // exactly 2 CDC nr (1,2) are valid!
    usb_buffer *ub = &usbdata[cdcNr-1];
    if (ub->con) {
        tud_cdc_n_write_flush(cdcNr); 
    }
}



int Usb::readNextRxByte(uint8_t nr) {
    assert((nr>=1) && (nr<=2));         // exactly 2 CDC nr (1,2) are valid!
    uint8_t databyte;
    usb_buffer *ub = &usbdata[nr-1];
    if (ub->ReadIdx == ub->RxIdx) {
        return READ_NODATA;
    }
    databyte = ub->RxBuffer[ub->ReadIdx++];
    if (ub->ReadIdx == BUFFER_SIZE) {
       ub->ReadIdx = 0;
    }
    return (int)databyte;
}


void Usb::executeCommand(int cmd, int argc, char** argv) {
}

//  ------- C API  -------------
Usb* createInstanceUsb() {
    return new Usb();
}

void SetUartProxy(Usb *instance,  UartBase *pCdc1Uart, UartBase *pCdc2Uart) {
    instance->SetUartProxy(pCdc1Uart, pCdc2Uart);
}