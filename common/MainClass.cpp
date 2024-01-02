// This is C++ code
#include "MainClass.h"
#include <iostream>
//#include <tusb.h>

using std::list;

MainClass::MainClass(std::list<Module*> ml): modules(ml) { }

void MainClass::addModule(Module *mod) {
    list<Module*>::iterator it1 = MainClass::modules.begin();
    modules.insert(it1, mod); 
}

// void MainClass::addModule(LedControl *mod) {
//     list<Module>::iterator it1 = MainClass::modules.begin();

//     // Warum liefert das keinen compile error !!!!!
//     // modules.insert(it1,mod);
//     // ????????????????????????????????????????????
//     modules.insert(it1,*mod); 
// }


void MainClass::runLoop() {
    int i = 0;
    std::list<Module*>::iterator it;

    for (it = modules.begin(); it != modules.end(); it++) {
        // Access the object through iterator
        //std::string name = (*it)->Name;
        //std::cout << "  " << name << std::endl;
        (*it)->init(nullptr);
    }

    while (true) {
        	
        //tud_task();
            
        for (it = modules.begin(); it != modules.end(); it++) {
            // Access the object through iterator  
            (*it)->main();        
        }
    }
}

//  ------- C API  -------------
MainClass* createInstanceMainClass() {
    std::list<Module*> modl;
    return new MainClass(modl);
}

void runLoopMainClass(MainClass* fred)
{
  fred->runLoop();
}

void addModuleMainClass(MainClass* mc, Module* mod) {
  mc->addModule(mod);
}
