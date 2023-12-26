// This is C++ code
#include "Module.h"

#include <string>
#include <iostream>
 

Module::Module(char *p) :pInitdata(p) { 
    std::string conv(p);
    //std::cout << "Const" << conv << std::endl;
    //std::string conv = *((std::string*)p);
    Name = conv;    
}

void Module::init(void *p)  { 

} 

void Module::main()  { 
    if (loopCnt > 0) {
        loopCnt--;
        if (loopCnt==0) {
            loopCnt = 5000000;
            std::cout << Name[0] << std::endl;
        }
    }
   
}


//  ------- C API  -------------
Module* createInstanceModule(char* pName)
{
//   std::string conv(pName);
//   std::cout << "create" << conv << std::endl;
  return new Module(pName);
}

