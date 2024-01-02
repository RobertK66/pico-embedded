// This is C++ code
#include "LedControl.h"

#include <iostream>
#include "pico/stdlib.h"

#define LED_PIN_RED     18
#define LED_PIN_GREEN   19
#define LED_PIN_BLUE    20

LedControl::LedControl(void) : Module::Module((char *)"LedControl") { 
}

void LedControl::init(void *p) {
    gpio_init(LED_PIN_RED);
	gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_init(LED_PIN_GREEN);
	gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_init(LED_PIN_BLUE);
	gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);
    gpio_put(LED_PIN_RED, 1);
    gpio_put(LED_PIN_GREEN, 1);
    gpio_put(LED_PIN_BLUE, 1);
    rval = 10;
    gval = 10;
    bval = 10;
}

void LedControl::main() { 
    if (loopCnt <= 100) {
        loopCnt++;
        if (loopCnt==100) {
            loopCnt = 0;
            if (rval>0) {
                gpio_put(LED_PIN_RED, 0);
            }
            if (gval>0) {
                gpio_put(LED_PIN_GREEN, 0);
            }
            if (bval>0) {
                gpio_put(LED_PIN_BLUE, 0);
            }
        }
        if (loopCnt == rval) {
            gpio_put(LED_PIN_RED, 1);
        }
        if (loopCnt == gval) {
            gpio_put(LED_PIN_GREEN, 1);
        }
        if (loopCnt == bval) {
            gpio_put(LED_PIN_BLUE, 1);
        }
    }
}

void LedControl::executeCommand(int cmd, int argc, char** argv) {
    int v1, v2, v3 = 0;
    if (argc>0) {
        v1 = atoi(argv[0]);
    }
    if (argc>1) {
        v2 = atoi(argv[1]);
    }
    if (argc>2) {
        v3 = atoi(argv[2]);
    }
    switch (cmd) {
        case CMD_LED_FLASH:
            break;
        case CMD_LED_RGB:
            rval = v1;
            gval = v2;
            bval = v3;
            break;
        default:
            break;
    }
}

//  ------- C API  -------------
LedControl* createInstanceLedControl(void) {
    return  new LedControl();
}