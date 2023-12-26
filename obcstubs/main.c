/**
 * Copyright (c) 2022 RKOS-IT
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
//#include "pico/multicore.h"


#include "../common/MainClass.h"
#include "../common/Module.h"
#include "../common/CliModule.h"
#include "mod/LedControl.h"

// As a Rasperry PICO W board does not have a GPIO LED this has to be 'masked here' if used on W-target
// #ifdef PICO_DEFAULT_LED_PIN
// 	#define LED_ON  gpio_put(PICO_DEFAULT_LED_PIN, 1)
// 	#define LED_OFF gpio_put(PICO_DEFAULT_LED_PIN, 0)
// #else
// 	#define LED_ON  {}
// 	#define LED_OFF {}
// #endif

// typedef enum {
// 	LEDs_Off,
// 	LEDs_On,
// 	LEDs_Slow,
// 	LEDs_Medium,
// 	LEDs_Fast
// } led_status;

// bool ledOn = true;
// uint ledCnt = 0;
// uint ledSpeed = 500000;

// void LEDOnOff(int argc, char* argv[]) {
// 	led_status stat = LEDs_Slow;
// 	if (argc >= 1) {
// 		stat = atoi(argv[0]);
// 	}
// 	ledSpeed = 0;
// 	printf("\nLed switched to ");

// 	switch (stat) {
// 	case LEDs_Off:
// 		LED_OFF;
// 		printf("off");
// 		break;
// 	case LEDs_On:
// 		LED_ON;
// 		printf("on");
// 		break;
// 	case LEDs_Slow:
	
// 		ledSpeed = 5000000;
// 		printf("slow");
// 		break;
// 	case LEDs_Medium:
// 		ledSpeed = 400000;
// 		printf("med");
// 		break;
// 	case LEDs_Fast:
// 		ledSpeed = 100;
// 		printf("fast");
// 		break;

// 	default:	
// 	    ledSpeed = 100001;
// 		//printf("make assert now");
// 		//assert(5 > 10);
// 	}
// 	ledCnt = ledSpeed;
// 	printf("\n");
// }

// void core1_main() {
// 	while (true) {
// 		// Process all registered CLI Commands
// 		CliMain();
// 		srsMain();
// 	}
// }


//bool usbConnected = false;


int main() {

	stdio_init_all();
	// read junk characters out of buffer !?
	//while ((getchar_timeout_us(5)) != PICO_ERROR_TIMEOUT);

	printf("Hello OBC Stubs\n");

	CliModule *cliMod = createInstanceCliModule();
	Module *ledMod = (Module*)createInstanceLedControl();

	cliRegisterCommand(cliMod, "led", ledMod, 1);

	MainClass *main0 = createInstanceMainClass();
	addModuleMainClass(main0, ledMod);
	addModuleMainClass(main0, (Module*)cliMod);

	runLoopMainClass(main0);


}
