// This is C++ code
#include "CliModule.h"

#include "string.h"
#include <iostream>
#include "pico/stdlib.h"

struct Cmd {
    const char*cmdString;
    Module* target;
    int cmdNumber;
};

CliModule::CliModule(void) : Module::Module((char *)"CliModule") { 

}

void CliModule::init(void *p) {
    // read junk characters out of uart buffer
	while ((getchar_timeout_us(50000)) != PICO_ERROR_TIMEOUT);
    rxPtrIdx = 0;
}


void CliModule::main() { 
    //#ifdef PICO_STDIO_USB_DEFAULT_CRLF 				// any other check avail to see if STDIO-usb is linked !?
    // Check if USB connection successfull
    if (usbConnected != stdio_usb_connected()) {
        usbConnected = stdio_usb_connected();
        if (usbConnected) {
            std::cout << "usb connected now!" << std::endl;
        } else {
            std::cout << "usb disconnected now!" << std::endl;
        }
    }
    //#endif

    int ch;
		
	while ((ch = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
		if (ch != 0x0a &&
			ch != 0x0d) {
			rxBuffer[rxPtrIdx++] = (char)(ch);
		}

		if ((rxPtrIdx >= CLI_RXBUFFER_SIZE - 1) ||
			 ch == 0x0a) {
			rxBuffer[rxPtrIdx] = 0x00;
			processLine();
			rxPtrIdx = 0;
		}
	}
}

void CliModule::processLine() { 
    bool processed = false;
    // first lets copy the received line to our command line
    cmdLine = std::string(rxBuffer);

	// Then we split for parameters
	int parCnt = 0;
	char* pars[20];
	for (int i = 0; i < 20; i++) {
		pars[i] = NULL;
	}
	for (int i = 0, p = 0; cmdLine[i] != 0x00; i++) {
		if ((cmdLine[i] == ' ') &&
			(p < 20)) {
			cmdLine[i] = 0x00;
			pars[p++] = (&cmdLine[i]) + 1;
			parCnt++;
		}
	}

	// Check if command can be found in definitions
    std::list<Cmd*>::iterator it;
    for (it = commands.begin(); it != commands.end(); it++) {
        if (strcmp((*it)->cmdString, &cmdLine[0]) == 0) {	
			// Call this command with the found params
            (*it)->target->executeCommand((*it)->cmdNumber, parCnt, pars);
			processed = true;
		}

    }

	if (!processed) {
		if (strlen(&cmdLine[0]) > 0) {
			printf("Command '%s' not found." , &cmdLine[0]);
			// for (int cmd = 0; cmd < cliRegisteredCommands; cmd++) {
			// 	printf("'%s' ", commands[cmd].cmdStr);
			// }
		}
		printf("\n");
	}

   // std::cout << rxBuffer << std::endl;
}



int CliModule::addCommand(const char*cmd, Module* target, int cmdNr) {
    Cmd *nc = new Cmd;
    nc->cmdString = cmd;
    nc->target = target;
    nc->cmdNumber = cmdNr;

    std::list<Cmd*>::iterator it1 = CliModule::commands.begin();
    commands.insert(it1, nc); 

    return cmdNr;
}

void CliModule::executeCommand(int nr, int cnt, char** par) {
    
}


//  ------- C API  -------------
CliModule* createInstanceCliModule(void) {
    return  new CliModule();
}

int cliRegisterCommand(CliModule* m, const char*cmd, Module* target, int cnr) {
    return m->addCommand(cmd, target, cnr);
}