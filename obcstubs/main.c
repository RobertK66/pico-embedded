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
#include "mod/UartProxy.h"

// local function prototypes
void main_core1();		

// Core 0 Modules
CliModule *cliMod;
Usb *myUsbMod;

// Core 1 Modules
LedControl *ledMod;
UartProxy  *uartAProxy;
UartProxy  *uartBProxy;

uart_config uc0 = { .uart = uart0, .rxPin=1, .txPin=0, .bit_rate=9600 };
uart_config uc1 = { .uart = uart1, .rxPin=5, .txPin=4, .bit_rate=9600 };	

int main() {
	// This inits are not done in module init because the sequence is important here!!!!
	tusb_init();		// The usb device is configured to have 3 CDC - Serial endpoints
	stdio_init_all();	// STDIO uses CDC0, CDC1/CDC2 are used as UART proxies for uart0/1

	// Create and wire up all modules.
	cliMod = createInstanceCliModule();
	ledMod = createInstanceLedControl();
	myUsbMod = createInstanceUsb();
	uartAProxy = createInstanceUartProxy(&uc0, "TT64-A", 1, myUsbMod);
	uartBProxy = createInstanceUartProxy(&uc1, "TT64-C", 2, myUsbMod);
	SetUartProxy(myUsbMod,(UartBase*)uartAProxy, (UartBase*)uartBProxy);
	
	// Register all module CLI Commands
	cliRegisterCommand(cliMod, "led",(Module*)ledMod, CMD_LED_FLASH);
	cliRegisterCommand(cliMod, "rgb",(Module*)ledMod, CMD_LED_RGB);

	// Prepare Core-0 MainLoop
	MainClass *main0 = createInstanceMainClass();
	addModuleMainClass(main0, (Module*)cliMod);
	addModuleMainClass(main0, (Module*)myUsbMod);

	// Start both main loops	
	multicore_launch_core1(main_core1); 
	runLoopMainClass(main0);
}

void main_core1() {
	// Prepare Core-1 MainLoop
	MainClass *main1 = createInstanceMainClass();
	addModuleMainClass(main1, (Module*)ledMod);
	addModuleMainClass(main1, (Module*)uartAProxy);
	addModuleMainClass(main1, (Module*)uartBProxy);
	runLoopMainClass(main1);
}
