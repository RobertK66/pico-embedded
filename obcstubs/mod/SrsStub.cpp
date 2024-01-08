// This is C++ code
#include "SrsStub.h"

#include <iostream>
#include <sstream>


SrsStub::SrsStub(uint8_t *txBuffer) : Module::Module((char *)"Srs")  { 
    pTxData = txBuffer;
    Message += "init";
}

void SrsStub::main() {
    if (Message.length() > 0) {
        std::cout << Message;
        Message = "";
    }
}

void SrsStub::ReceiveByte(uint8_t b, bool isAddress) {
    if (isAddress) {
        RxIdx = 0;
        if (b == SRS_POWER_ADR) {
            Status = RxPowerbus;
        } else if (b == SRS_CTRL_ADR) {
            Status = RxRequest;
        } else {
            // Other slave address !? -> not registered. So its a prog errro
            Message += "Wrong Slave adress\n";
        }
    } else {
        RxData[RxIdx] = b;
        if (Status == RxPowerbus) {
            if (RxIdx == 1) {
                if (b == 0x1F) {
                    Message += "Power ON\n"; 
                } else if (b == 0x17) {
                    Message += "Power OFF\n"; 
                } else {
                    Message += "unknown power cmd\n"; 
                }
            }
        } else if (Status == RxRequest) {

        } 
        RxIdx++;
        if (RxIdx>=SRS_BUFFER_SIZE) {
            RxIdx--;
            Message += "Rx Buffer overflow\n";
        }
    }
}

void SrsStub::GenerateAnswer(uint8_t address) {
    
    Message += "prep answer\n"; 
}


//  ------- C API  -------------
SrsStub* createInstanceSrsStub(uint8_t *txBuffer) {
    return new SrsStub(txBuffer);
}

 void srs_receive_handler(SrsStub* inst, uint8_t data, bool is_address) {
    inst->ReceiveByte(data, is_address);
 }

 void srs_request_handler(SrsStub* inst, uint8_t address) {
    inst->GenerateAnswer(address);
 }

//  void srs_stop_handler(SrsStub* inst, uint8_t length) {

//  }
