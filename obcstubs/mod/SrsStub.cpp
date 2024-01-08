// This is C++ code
#include "SrsStub.h"

#include <iostream>
#include <sstream>
#include <cstring>

#define SRS_CTRLCMD_SETSYNCTIME		0x0D
#define SRS_CTRLCMD_REQSTATUS		0x0A
#define SRS_CTRLCMD_SETINTERVALS	0x0B
#define SRS_CTRLCMD_REQINTERVALS	0x0C
#define SRS_CTRLCMD_REQSYNCTIME		0x0E
#define SRS_CTRLCMD_SHUTDOWN		0x0F
#define SRS_CTRLCMD_REQDATAADDR		0x10
#define SRS_CTRLCMD_REQDATA			0x11

#define SRS_STATUSTYPE_COMMON		0x01
#define SRS_STATUSTYPE_EXTENDED		0x02
#define SRS_STATUSTYPE_BOTH			0x03
#define SRS_STATUSTYPE_INVALID		0xFF

#define SRS_INTERVAL_FGDOS		    0x01
#define SRS_INTERVAL_RADFET		    0x02
#define SRS_INTERVAL_SRAM		    0x03

#define SRS_DATACMD_INIT			0x00
#define SRS_DATACMD_TRANSFER		0x01
#define SRS_DATACMD_RESEND			0x02
#define SRS_DATACMD_STOP			0x03

#define SRS_DATARESP_BLOCKSIZEERROR		0x15
#define SRS_DATARESP_TRANSFERROR		0x14	// 'Not Initialized Error'
#define SRS_DATARESP_NOMMOREDATA		0x16


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
            Message += "Wrong Slave address!?\n";
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
                    Message += "Unknown power cmd\n"; 
                }
            }
        } else if (Status == RxRequest) {
            // nothing to do here
        } 
        RxIdx++;
        if (RxIdx>=SRS_BUFFER_SIZE) {
            RxIdx--;
            Message += "Rx Buffer overflow!\n";
        }
    }
}

void SrsStub::GenerateAnswer(uint8_t address) {
    std::ostringstream out;
    if (address == SRS_CTRL_ADR) {
        // Check Crc
        if (CalculateCrc(RxData, RxIdx) != 0x00) {
            out << "CRC Error\n";
            // CRC Error in Tx....
            pTxData[0] = 0xFF;
            pTxData[1] = 0x01;
            pTxData[2] = CalculateCrc(pTxData,2);
        } else {
            // Process Command and generate Answer
            switch (RxData[0]) {
            case SRS_CTRLCMD_SETSYNCTIME:
            {
                data.PosixTime = 0;
                for (int i=0; i<8; i++) {
                    *((uint8_t*)(&data.PosixTime) + i) = RxData[i+1];
                }
                out << "Time Sync received: " << data.PosixTime << "\n";
                // generate answer (copy of rx data)
                std::memcpy(pTxData, RxData, 10);
                break;
            }

            case SRS_CTRLCMD_REQSYNCTIME:
            {   
                pTxData[0] = SRS_CTRLCMD_REQSYNCTIME;
                for (int i=0; i<8; i++) {
                    pTxData[i+1] = *((uint8_t*)(&data.PosixTime) + i);
                }                
                pTxData[9] = CalculateCrc(pTxData, 9);
                out << "Time sent: " << data.PosixTime << "\n";
                break;
            }

            case SRS_CTRLCMD_SETINTERVALS:
            {
                uint32_t value = 0;
                for (int i=0; i<4; i++) {
                    *((uint8_t*)(&value) + i) = RxData[2+i];
                }
                if (RxData[1] == SRS_INTERVAL_FGDOS) {
                    data.intervalFgDos = value;
                } else if (RxData[1] == SRS_INTERVAL_RADFET) {
                    data.intervalRadFet = value;
                } else if (RxData[1] == SRS_INTERVAL_SRAM) {
                    data.intervalSram = value;
                } 
                out << "Interval type " << (int)(RxData[1]) << " set to "<< value <<"\n";
                // generate answer (copy of rx data)
                std::memcpy(pTxData, RxData, 7);
                break;
            }

            case SRS_CTRLCMD_REQINTERVALS:
            {
                int i=0;
                pTxData[i++] = SRS_CTRLCMD_REQINTERVALS;
                memcpy(&pTxData[i], &data.intervalFgDos, 4);
                i += 4;
                memcpy(&pTxData[i], &data.intervalRadFet, 4);
                i += 4;
                memcpy(&pTxData[i], &data.intervalSram, 4);
                i += 4;
                pTxData[i] = CalculateCrc(pTxData, i);
                out << "Intervalls sent: " << data.intervalFgDos << ", " << data.intervalRadFet << ", " << data.intervalSram << "\n";
                break;
            }

            case SRS_CTRLCMD_SHUTDOWN:
            {
                out << "Shutdown\n";
                break;
            }
            
            case SRS_CTRLCMD_REQSTATUS:
            {
                int i=0;
                pTxData[i++] = SRS_CTRLCMD_REQSTATUS;
                
                if (RxData[1] == SRS_STATUSTYPE_COMMON) {
                    pTxData[i++] = SRS_STATUSTYPE_COMMON; 
                    memcpy(&pTxData[i], data.commonStatus, 12);
                    i += 12;
                } else if (RxData[1] == SRS_STATUSTYPE_EXTENDED) {
                    pTxData[i++] = SRS_STATUSTYPE_EXTENDED; 
                    memcpy(&pTxData[i], data.extendedStatus, 30);
                    i += 30;
                }  else if (RxData[1] == SRS_STATUSTYPE_BOTH) {
                    pTxData[i++] = SRS_STATUSTYPE_BOTH; 
                    memcpy(&pTxData[i], data.commonStatus, 12);
                    i += 12;
                    memcpy(&pTxData[i], data.extendedStatus, 30);
                    i += 30;
                } else {
                    pTxData[i++] = SRS_STATUSTYPE_INVALID; 
                }
                pTxData[i] = CalculateCrc(pTxData, i);
                out << i << " Bytes Status data sent.\n";
                break;
            }
    
        // #define SRS_CTRLCMD_REQDATAADDR		0x10
        // #define SRS_CTRLCMD_REQDATA		  
            default:
                out << "Unknown command\n"; 
            }
        }
    } else {
        out << "Address changed!?\n"; 
    }
    Message += out.str();
}


uint8_t SrsStub::CalculateCrc(uint8_t *data, int len) {
    uint8_t ret = 0x00;
	while (len>0) {
		ret ^= data[--len];
	}
	return ret;
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
