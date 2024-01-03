/**
 * Copyright (c) 2022 RKOS-IT
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <tusb.h>
#include "pico/multicore.h"

#include "../common/MainClass.h"
#include "../common/Module.h"
#include "../common/CliModule.h"
#include "mod/LedControl.h"
#include "mod/Usb.h"

CliModule *cliMod;
Module *myUsbMod;
Module *ledMod;

void main_core1() {
	MainClass *main1 = createInstanceMainClass();
	addModuleMainClass(main1, ledMod);
	runLoopMainClass(main1);
}

int main() {
	// This inits arenot done in module init because the sequence is important here!!!!
	tusb_init();		// The usb device is configured to have 3 CDC - Serial endpoints
	stdio_init_all();	// STDIO uses CDC0, CDC1/CDC2 are used as UART proxies for uart0/1
	
	cliMod = createInstanceCliModule();
	ledMod = (Module*)createInstanceLedControl();
	myUsbMod = (Module*)createInstanceUsb();

	cliRegisterCommand(cliMod, "led", ledMod, CMD_LED_FLASH);
	cliRegisterCommand(cliMod, "rgb", ledMod, CMD_LED_RGB);

	MainClass *main0 = createInstanceMainClass();
	addModuleMainClass(main0, (Module*)cliMod);
	addModuleMainClass(main0, myUsbMod);

	// Start both main loops	
	multicore_launch_core1(main_core1); 
	runLoopMainClass(main0);
}
