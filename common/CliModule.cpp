// This is C++ code
#include "CliModule.h"

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
   
}

void CliModule::main() { 
   
}

int CliModule::addCommand(const char*cmd, Module* target, int cmdNr) {
    Cmd nc;
    nc.cmdString = cmd;
    nc.target = target;
    nc.cmdNumber = cmdNr;

    std::list<Cmd*>::iterator it1 = CliModule::commands.begin();
    commands.insert(it1, &nc); 

    return cmdNr;
}

//  ------- C API  -------------
CliModule* createInstanceCliModule(void) {
    return  new CliModule();
}

int cliRegisterCommand(CliModule* m, const char*cmd, Module* target, int cnr) {
    return m->addCommand(cmd, target, cnr);
}