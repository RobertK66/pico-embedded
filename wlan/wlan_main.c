/**
 * Copyright (c) 2022 RKOS-IT
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

// only available for WLAN board def!
//#include "pico/cyw43_arch.h"
//#include <lwip/sockets.h>

#ifdef PICO_DEFAULT_LED_PIN
#define LED_ON  gpio_put(PICO_DEFAULT_LED_PIN, 1)
#define LED_OFF gpio_put(PICO_DEFAULT_LED_PIN, 0)
#else
#define LED_ON  {}
#define LED_OFF {}
#endif

typedef enum {
	LEDs_Off,
	LEDs_On,
	LEDs_Slow,
	LEDs_Medium,
	LEDs_Fast
} led_status;

bool ledOn = true;
uint ledCnt = 0;
uint ledSpeed = 5000000;


void LEDOnOff(int argc, char* argv[]) {
	led_status stat = LEDs_Slow;
	if (argc >= 1) {
		stat = atoi(argv[0]);
	}
	ledSpeed = 0;
	printf("\nLed switched to ");

	switch (stat) {
	case LEDs_Off:
		LED_OFF;
		printf("off");
		break;
	case LEDs_On:
		LED_ON;
		printf("on");
		break;
	case LEDs_Slow:
	
		ledSpeed = 5000000;
		printf("slow");
		break;
	case LEDs_Medium:
		ledSpeed = 400000;
		printf("med");
		break;
	case LEDs_Fast:
		ledSpeed = 100;
		printf("fast");
		break;

	default:	
	    ledSpeed = 100001;
		printf("make assert now");
		assert(5 > 10);
	}
	ledCnt = ledSpeed;
	printf("\n");
}




void core1_main() {


    while (true)
    {
    }

}


bool usbConnected = false;


int main() {

	stdio_init_all();
	// read junk characters out of buffer 
	while ((getchar_timeout_us(50000)) != PICO_ERROR_TIMEOUT);

	// if (cyw43_arch_init_with_country(CYW43_COUNTRY_AUSTRIA)) {
	// 	printf("Hello wlan 1\n");
	// 	printf("failed to initialise\n");
	// } else {
	// 	printf("Hello wlan 2\n");
	// 	printf("initialised\n");
	// }
	 
	// cyw43_arch_enable_sta_mode();
	 
	// if (cyw43_arch_wifi_connect_timeout_ms("Christofer", "**********", CYW43_AUTH_WPA2_AES_PSK, 10000)) {
	// 	printf("Hello wlan 3\n");
	// 	printf("failed to connect\n");
	// } else {
	// 	printf("Hello wlan 4\n");
	// 	printf("connected\n");
	// }

	

    //gpio_init(PICO_DEFAULT_I2C_SCL_PIN);     // Pin 5
    //gpio_init(PICO_DEFAULT_I2C_SDA_PIN);     // Pin 4
    //gpio_set_dir(PICO_DEFAULT_I2C_SCL_PIN, GPIO_OUT);
    //gpio_set_dir(PICO_DEFAULT_I2C_SDA_PIN, GPIO_OUT);



	
	// read junk characters out of buffer 
	//while ((getchar_timeout_us(50000)) != PICO_ERROR_TIMEOUT);

	//CliInit();
	//icsInit();

	multicore_launch_core1(core1_main); 

	ledCnt = ledSpeed;

	//printf("Hello wlan xyz\n");
	//CliRegisterCommand("led", LEDOnOff);
	
	while (true) {

		#ifdef PICO_STDIO_USB_DEFAULT_CRLF 				// any other check avail to see if STDIO-usb is linked !?
		// Check if USB connection successfull
		if (usbConnected != stdio_usb_connected()) {
			usbConnected = stdio_usb_connected();
			if (usbConnected) {
				printf("\nusb connected now!\n");
			}
			else {
				printf("\nusb disconnected now!\n");
			}
		}
		#endif

		// Control the LED blink speed feature
		if (ledCnt > 0) {
			ledCnt--;
			if (ledCnt <= 0) {
				if (ledOn) {
					ledOn = false;
					gpio_put(PICO_DEFAULT_I2C_SDA_PIN, 0);
				    gpio_put(PICO_DEFAULT_I2C_SCL_PIN, 1);
//					cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
					printf("on\n");
						
					
				}
				else {
					ledOn = true;
					gpio_put(PICO_DEFAULT_I2C_SDA_PIN, 1);
					gpio_put(PICO_DEFAULT_I2C_SCL_PIN, 0);
//					cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
					printf("off\n");
				}
				ledCnt = ledSpeed;
			}
		}

		// Process all registered CLI Commands
		//CliMain();
	}
}
