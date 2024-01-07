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

int main() {
	// This inits are not done in module init because the sequence is important here!!!!
	tusb_init();		// The usb device is configured to have 3 CDC - Serial endpoints
	stdio_init_all();	// STDIO uses CDC0, CDC1/CDC2 are used as UART proxies for uart0/1

	// Create and wire up all modules.
	cliMod = createInstanceCliModule();
	ledMod = createInstanceLedControl();
	myUsbMod = createInstanceUsb();
	uart_config c0;
	c0.uart=uart0;
	c0.bit_rate=9600;
	c0.rxPin=1;
	c0.txPin=0;
	uartAProxy = createInstanceUartProxy(&c0, "TT64-A", 1, myUsbMod);
	uart_config c1;
	c1.uart=uart1;
	c1.bit_rate=9600;
	c1.rxPin=5;
	c1.txPin=4; 
	uartBProxy = createInstanceUartProxy(&c1, "TT64-C", 2, myUsbMod);
	SetUartProxy(myUsbMod,(UartBase*)uartAProxy, (UartBase*)uartBProxy);
	
	// Register all module CLI Commands
	cliRegisterCommand(cliMod, "led",(Module*)ledMod, CMD_LED_FLASH);
	cliRegisterCommand(cliMod, "rgb",(Module*)ledMod, CMD_LED_RGB);


	// Prepare Core0 MainLoop
	MainClass *main0 = createInstanceMainClass();
	addModuleMainClass(main0, (Module*)cliMod);
	addModuleMainClass(main0, (Module*)myUsbMod);

	// Start both main loops	
	multicore_launch_core1(main_core1); 
	runLoopMainClass(main0);
}

void main_core1() {
	MainClass *main1 = createInstanceMainClass();
	addModuleMainClass(main1, (Module*)ledMod);
	addModuleMainClass(main1, (Module*)uartAProxy);
	addModuleMainClass(main1, (Module*)uartBProxy);
	runLoopMainClass(main1);
}
