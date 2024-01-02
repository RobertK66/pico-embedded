// This is C++ code
#include "Usb.h"

#include <iostream>
#include <stdlib.h>
#include <tusb.h>


Usb::Usb(void) : Module::Module((char *)"Usb") { 
}

void Usb::init(void *p) {
    //tusb_init(); this has to be done before stdio_init !!!
}

void Usb::main() { 
    tud_task();
}

void Usb::executeCommand(int cmd, int argc, char** argv) {
}

//  ------- C API  -------------
Usb* createInstanceUsb(void) {
    return new Usb();
}