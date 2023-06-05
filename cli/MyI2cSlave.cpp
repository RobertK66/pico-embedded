#include "MyI2cSlave.hpp"

#include <stdio.h>
#include <stdexcept>


class MyI2cSlave {
    public:
        MyI2cSlave();
        MyI2cSlave(int a, int s);
    private:
        int adr;
    
};

MyI2cSlave::MyI2cSlave(int a, int s) {
    adr = a;
    if (a == 42) {
      throw std::invalid_argument( "received the wrong adr value" );
    }
} 

MyI2cSlave::MyI2cSlave() {
    adr = 0x20;
}


EXTERNC rk_myI2cSlave_t rk_create_myI2cSlave() NOTHROW {
   try {
     return static_cast<rk_myI2cSlave_t>(new MyI2cSlave);
   } catch (std::invalid_argument& ex) {
     printf(ex.what());
     return nullptr;
   }
 }

 EXTERNC rk_myI2cSlave_t rk_create_myI2cSlave_with_int(int adr, int size) NOTHROW {
   try {
     return static_cast<rk_myI2cSlave_t>(new MyI2cSlave(adr, size));
    }
    catch (std::invalid_argument& ex) {
      printf(ex.what());
      return nullptr;
    }
 }

 EXTERNC void rk_free_myI2cSlave(rk_myI2cSlave_t obj) NOTHROW {
   try {
     MyI2cSlave* typed_obj = static_cast<MyI2cSlave*>(obj);
     delete typed_obj;
    }
    catch (...) {
        // Ignore
    }
 }