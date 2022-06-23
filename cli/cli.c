/**
 * Copyright (c) 2022 RKOS-IT
 */

#include <stdio.h>
#include "pico/stdlib.h"

#ifdef PICO_DEFAULT_LED_PIN
    #define LED_ON  gpio_put(PICO_DEFAULT_LED_PIN, 1)
    #define LED_OFF gpio_put(PICO_DEFAULT_LED_PIN, 0)
#else
    #define LED_ON  {}
    #define LED_OFF {}
#endif

int main() {
#ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif
    stdio_init_all();

    while (true) {
        /*LED_ON;
        printf("Hello CLI");
        sleep_ms(500);
        LED_OFF;
        sleep_ms(500);
       */
        int c = getchar_timeout_us(0);
            
        if (c != PICO_ERROR_TIMEOUT) {
            printf("Rx: '%c'\r", c);
        }
            

    }
}

