// This is C++ code
#include "Usb.h"

#include <iostream>
#include <stdlib.h>
#include <tusb.h>


Usb::Usb(void) : Module::Module((char *)"Usb") { 
    usbdata[0].con = false;
    usbdata[0].RxIdx = 0;
    usbdata[1].con = false;
    usbdata[1].RxIdx = 0;
}

void Usb::init(void *p) {
    //tusb_init(); this has to be done before stdio_init !!!
}

void Usb::main() { 
    tud_task(); // This serves the tiny USB main task to establish CDCs 
                // on USB composite device.
    for (int nr=1; nr<=2; nr++) {
        bool c = tud_cdc_n_connected(nr);
        usb_buffer *ub = &usbdata[nr-1];
        if (ub->con != c) {
            ub->con = c;
            if (ub->con) {
                cdc_line_coding_t lc;
                tud_cdc_n_get_line_coding(nr, &lc);
                printf("COM-'%i' connected with %i,%i,%i,%i\n", 
                            nr, lc.bit_rate, lc.data_bits, lc.stop_bits, lc.parity);
            } else {
                printf("COM-'%i' disconnected.\n", nr);
            }
        }
        if (ub->con) {
            uint32_t len = tud_cdc_n_available(nr);
            if (len>0) {
                uint32_t len1 = MIN(len, BUFFER_SIZE-ub->RxIdx);
                ub->RxIdx += tud_cdc_n_read(nr, &(ub->RxBuffer[ub->RxIdx]), len1);
                if (len1<len) {
                    ub->RxIdx = tud_cdc_n_read(nr, &(ub->RxBuffer[0]), len - len1);
                }
                printf("Rx[%i] %i bytes. Buffered Up to: %i\n", nr, len, ub->RxIdx);
            }
        }
    }
}

void Usb::executeCommand(int cmd, int argc, char** argv) {
}

//  ------- C API  -------------
Usb* createInstanceUsb(void) {
    return new Usb();
}