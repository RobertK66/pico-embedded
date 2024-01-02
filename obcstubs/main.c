/**
 * Copyright (c) 2022 RKOS-IT
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <tusb.h>
//#include "pico/multicore.h"

#include "../common/MainClass.h"
#include "../common/Module.h"
#include "../common/CliModule.h"
#include "mod/LedControl.h"
#include "mod/Usb.h"

int main() {
	// This inits arenot done in module init because the sequence is important here!!!!
	tusb_init();		// The usb device is configured to have 3 CDC - Serial endpoints
	stdio_init_all();	// STDIO uses CDC0, CDC1/CDC2 are used as UART proxies for uart0/1
	
	//printf("Hello OBC Stubs\n");

	CliModule *cliMod = createInstanceCliModule();
	Module *ledMod = (Module*)createInstanceLedControl();
	Module *myUsbMod = (Module*)createInstanceUsb();

	cliRegisterCommand(cliMod, "led", ledMod, CMD_LED_FLASH);
	cliRegisterCommand(cliMod, "rgb", ledMod, CMD_LED_RGB);

	MainClass *main0 = createInstanceMainClass();
	addModuleMainClass(main0, ledMod);
	addModuleMainClass(main0, (Module*)cliMod);
	addModuleMainClass(main0, myUsbMod);
	

	runLoopMainClass(main0);


}
