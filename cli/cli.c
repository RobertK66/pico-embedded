/**
 * Copyright (c) 2022 RKOS-IT
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "tusb.h"
#include "pico/stdlib.h"

#ifdef PICO_DEFAULT_LED_PIN
    #define LED_ON  gpio_put(PICO_DEFAULT_LED_PIN, 1)
    #define LED_OFF gpio_put(PICO_DEFAULT_LED_PIN, 0)
#else
    #define LED_ON  {}
    #define LED_OFF {}
#endif

// Command Interface
#define C_MAX_CMDSTR_LEN		16
#define CLI_PROMPT 				">"
#define CLI_MAX_COMMANDS		100
#define CLI_MAX_PARAMS			16


typedef struct cliCommand {
    char	cmdStr[C_MAX_CMDSTR_LEN];
    void (*func)(int argc, char* argv[]);
} cliCommand_t;

// The Rx line buffer - used with polling from mainloop
#define CLI_RXBUFFER_SIZE 128
char cliRxBuffer[CLI_RXBUFFER_SIZE];
int cliRxPtrIdx = 0;

// Command Line parsing and registry
char cmdLine[CLI_RXBUFFER_SIZE + 10];
int  cliRegisteredCommands = 0;
cliCommand_t commands[CLI_MAX_COMMANDS];

// module statistics
int linesProcessed = 0;
int cmdsProcessed = 0;

//
// module prototypes
// ----------------------
void processLine();
void CliShowStatistics(int argc, char* argv[]);

//
// Module function implementations
// -------------------------------

// With this function you can register your custom command handler(s)
void CliRegisterCommand(char* cmdStr, void (*callback)(int argc, char* argv[])) {
	// TODO check if duplicate entry !!!
	if (cliRegisteredCommands < CLI_MAX_COMMANDS) {
		strncpy(commands[cliRegisteredCommands].cmdStr, cmdStr, C_MAX_CMDSTR_LEN);
		commands[cliRegisteredCommands].func = callback;
		cliRegisteredCommands++;
	}
	else {
		printf("No Command slot left for registering new command.");
	}
}

// The module init can be called more than once. Last one wins with registered cliCommands only.
void CliInit() {
	// Clear all the registered commands.
	for (int i = 0; i < CLI_MAX_COMMANDS; i++) {
		commands[i] = (const struct cliCommand){ 0 };
	}
	cliRegisteredCommands = 0;
	
	CliRegisterCommand("cliStat", CliShowStatistics);
	printf(CLI_PROMPT);
}

// This is module main loop entry. Do not use (too much) time here!!!
void CliMain() {
	int ch;
		
	while ((ch = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
		if (ch != 0x0a &&
			ch != 0x0d) {
			cliRxBuffer[cliRxPtrIdx++] = (char)(ch);
		}

		if ((cliRxPtrIdx >= CLI_RXBUFFER_SIZE - 1) ||
			ch == 0x0a) {
			cliRxBuffer[cliRxPtrIdx] = 0x00;
			processLine();
			cliRxPtrIdx = 0;
			printf(CLI_PROMPT);
		}
	}

}


void processLine() {
	bool processed = false;
	linesProcessed++;

	// first lets copy the received line to our command line
	strcpy(cmdLine, cliRxBuffer);

	// Then we split for parameters
	int parCnt = 0;
	char* pars[CLI_MAX_PARAMS];
	for (int i = 0; i < CLI_MAX_PARAMS; i++) {
		pars[i] = NULL;
	}
	for (int i = 0, p = 0; cmdLine[i] != 0x00; i++) {
		if ((cmdLine[i] == ' ') &&
			(p < CLI_MAX_PARAMS)) {
			cmdLine[i] = 0x00;
			pars[p++] = (&cmdLine[i]) + 1;
			parCnt++;
		}
	}

	// Check if command can be found in definitions
	for (int cmd = 0; cmd < cliRegisteredCommands; cmd++) {
		if (strcmp(commands[cmd].cmdStr, &cmdLine[0]) == 0) {	// Todo: cut whitespaces at begin....
			// Call this command with the found params
			commands[cmd].func(parCnt, pars);
			processed = true;
			cmdsProcessed++;
		}
	}

	if (!processed) {
		if (strlen(&cmdLine[0]) > 0) {
			printf("Command '%s' not found. Try one of these:\n", &cmdLine[0]);
			for (int cmd = 0; cmd < cliRegisteredCommands; cmd++) {
				printf("'%s' ", commands[cmd].cmdStr);
			}
		}
		printf("\n");
	}

}

void CliShowStatistics(int argc, char* argv[]) {
	printf("CliShowStatistics called with\n");
	for (int i = 0; i < argc; i++) {
		printf("p-%d %s\n", i, argv[i]);
	}

	printf("\nlinesProcessed: %d\ncmdsProcessed: %d\n", linesProcessed, cmdsProcessed);
}

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
	switch (stat) {
	case LEDs_Off:
		LED_OFF;
		break;
	case LEDs_On:
		LED_ON;
		break;
	case LEDs_Slow:
	default:
		ledSpeed = 50000;
		break;
	case LEDs_Medium:
		ledSpeed = 10000;
		break;
	case LEDs_Fast:
		ledSpeed = 10;
		break;
	}
	ledCnt = ledSpeed;
}


bool usbConnected = false;

int main() {
#ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif
    stdio_init_all();
	// read chunk characters out of buffer
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
    } else {
        printf("\nNo connection on USB COM!\n");
    }

    while (true) {
		if (usbConnected != stdio_usb_connected()) {
			usbConnected = stdio_usb_connected();
			if (usbConnected) {
				printf("\nusb connected now.\n");
			}
			else {
				printf("\nusb disconnected now.\n");
			}
		}

		if (ledCnt > 0) {
			ledCnt--;
			if (ledCnt <= 0) {
				if (ledOn) {
					ledOn = false;
					LED_OFF;
				} else {
					ledOn = true;
					LED_ON;
				}
				ledCnt = ledSpeed;
			}
		}

		CliMain();
    }
}

