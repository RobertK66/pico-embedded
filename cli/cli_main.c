/**
 * Copyright (c) 2022 RKOS-IT
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "cli.h"

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
uint ledSpeed = 0;


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
	default:
		ledSpeed = 50000;
		printf("slow");
		break;
	case LEDs_Medium:
		ledSpeed = 10000;
		printf("med");
		break;
	case LEDs_Fast:
		ledSpeed = 10;
		printf("fast");
		break;
	}
	ledCnt = ledSpeed;
	printf("\n");
}


bool usbConnected = false;

int main() {
#ifdef PICO_DEFAULT_LED_PIN
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif
	stdio_init_all();
	// read junk characters out of buffer
	while ((getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT);

	CliInit();
	CliRegisterCommand("led", LEDOnOff);
	printf("Hello CLI\n");

#if TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX==1
	printf("USB_DEVICE_ENUMERATION_FIX is enabled\n");
#endif

	usbConnected = stdio_usb_connected();
	if (usbConnected) {
		printf("\nusb connected!\n");
	}
	else {
		printf("\nNo connection on USB COM!\n");
	}

	while (true) {
		// Check if USB connection successfull
		if (usbConnected != stdio_usb_connected()) {
			usbConnected = stdio_usb_connected();
			if (usbConnected) {
				printf("\nusb connected now.\n");
			}
			else {
				printf("\nusb disconnected now.\n");
			}
		}

		// Control the LED blink speed feature
		if (ledCnt > 0) {
			ledCnt--;
			if (ledCnt <= 0) {
				if (ledOn) {
					ledOn = false;
					LED_OFF;
				}
				else {
					ledOn = true;
					LED_ON;
				}
				ledCnt = ledSpeed;
			}
		}

		// Process all registered CLI Commands
		CliMain();
	}
}
