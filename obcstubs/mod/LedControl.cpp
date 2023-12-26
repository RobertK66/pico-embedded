// This is C++ code
#include "LedControl.h"

#include <iostream>
#include "pico/stdlib.h"

LedControl::LedControl(void) : Module::Module((char *)"LedControl") { 
}

void LedControl::init(void *p) {
    gpio_init(18);
	gpio_set_dir(18, GPIO_OUT);
    gpio_init(19);
	gpio_set_dir(19, GPIO_OUT);
    gpio_init(20);
	gpio_set_dir(20, GPIO_OUT);
    gpio_put(18, 1);
    gpio_put(19, 1);
    gpio_put(20, 1);
}

void LedControl::main() { 
    if (loopCnt > 0) {
        loopCnt--;
        if (loopCnt==0) {
            loopCnt = 3000000;
            gpio_put(18, 1);
            gpio_put(19, 0);
            std::cout << Name << std::endl;
        } else if(loopCnt == 2500000) {
            gpio_put(18, 0);
            gpio_put(19, 1);
        }
    }
}


//  ------- C API  -------------
LedControl* createInstanceLedControl(void) {
    return  new LedControl();
}