#include "srs_sim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h> //PRIu64

#include <pico/stdlib.h>
#include <hardware/i2c.h>

//#define ICS_MEMORY_SIZE 256

//#define SRS_CMD_SETSYNCTIME 0x0D


#define SRS_CTRLCMD_SETSYNCTIME		0x0D
//#define SRS_CMDEXEC_SETSYNCTIME		0x00000001

#define SRS_CTRLCMD_REQSTATUS		0x0A
#define SRS_STATUSTYPE_COMMON		0x01
#define SRS_STATUSTYPE_EXTENDED		0x02
#define SRS_STATUSTYPE_BOTH			0x03
#define SRS_STATUSTYPE_INVALID		0xFF
//#define SRS_CMDEXEC_REQSTATUS_C		0x00000002
//#define SRS_CMDEXEC_REQSTATUS_E		0x00000004
//#define SRS_CMDEXEC_REQSTATUS_B		0x00000008


#define SRS_CTRLCMD_SETINTERVALS	0x0B
#define SRS_STATUSTYPE_FGDOS		0x01
#define SRS_STATUSTYPE_RADFET		0x02
#define SRS_STATUSTYPE_SRAM			0x03
//#define SRS_CMDEXEC_SETINTV_F		0x00000010
//#define SRS_CMDEXEC_SETINTV_R		0x00000020
//#define SRS_CMDEXEC_SETINTV_S		0x00000040

#define SRS_CTRLCMD_REQINTERVALS	0x0C
//#define SRS_CMDEXEC_REQINTERVALS	0x00000080

#define SRS_CTRLCMD_REQSYNCTIME		0x0E
//#define SRS_CMDEXEC_REQSYNCTIME		0x00000100

#define SRS_CTRLCMD_SHUTDOWN		0x0F
//#define SRS_CMDEXEC_SHUTDOWN		0x00000200

#define SRS_CTRLCMD_REQDATAADDR		0x10
//#define SRS_CMDEXEC_REQDATAADDR		0x00000400

#define SRS_CTRLCMD_REQDATA			0x11
#define SRS_DATACMD_INIT			0x00
#define SRS_DATACMD_TRANSFER		0x01
#define SRS_DATACMD_RESEND			0x02
#define SRS_DATACMD_STOP			0x03

#define SRS_DATARESP_BLOCKSIZEERROR		0x15
#define SRS_DATARESP_TRANSFERROR		0x14	// 'Not Initialized Error'
#define SRS_DATARESP_NOMMOREDATA		0x16
//#define SRS_CMDEXEC_REQDATA			0x00000800 








#define SRS_POWER_ADR       0x47
#define SRS_CTRL_ADR        0x0B
#define SRS_MAX_CMD_SIZE    1024

// uint16_t memAddr = 0;
// uint8_t Memory[ICS_MEMORY_SIZE];

uint8_t UnknownErrorBuffer[SRS_MAX_CMD_SIZE] = { 0xFF, 0x00, 0xFF };
uint8_t CrcErrorBuffer[SRS_MAX_CMD_SIZE] = { 0xFF, 0x01, 0xFE };
uint8_t ParamErrorBuffer[SRS_MAX_CMD_SIZE] = { 0xFF, 0x02, 0xFD };
uint8_t ErrorInterval[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


uint32_t ErrorBits = 0;
#define ERROR_CRC           0x00000001
#define ERROR_UNKNOWN_CMD   0x00000002
#define ERROR_PARAM         0x00000004
#define ERROR_CMD_OVERFLOW  0x00000008

uint8_t RxBuffer[SRS_MAX_CMD_SIZE+1];
uint8_t RxCrc = 0;
uint16_t RxIndex = 0;
uint16_t RxExpectedLen = 0;

uint8_t TxBuffer[1000];
uint8_t *pTxBuffer = 0;
int16_t TxToSend = 0;


bool    CmdReceived = false;
uint8_t CmdBuffer[SRS_MAX_CMD_SIZE+1];
uint16_t CmdLen;


// Simulated Module state
static int64_t posixTim64 = 0;      // time does not change/count locally
static uint32_t intervalFgDos = 100;
static uint32_t intervalRadFet = 1000;
static uint32_t intervalSram = 10042;

static uint8_t commonStatus[12];
static uint8_t extendedStatus[30];

static uint32_t dataStartAddress = 4000; 
static uint32_t dataEndAddress = 4500;
static uint32_t bytesToSend = 0;
static uint32_t blocksToSend = 0;
static uint32_t blocksRequested = 0;
static uint16_t blockSize = 0;

static uint8_t crcErrorEvery = 0;
static uint16_t answerCnt = 0;

uint8_t  srsCrc(uint8_t *p, uint16_t len);
uint16_t SrscmdLength(uint8_t byte);
uint16_t GenerateAnswerIrq(uint8_t *CmdBuffer,uint16_t len, uint8_t **pTxBuffer);

int answerInitData(uint8_t **ppTxBuffer);
int answerNextDataBlock(uint8_t **ppTxBuffer);
int answerCurrentDataBlock(uint8_t **ppTxBuffer);
int answerStop(uint8_t **ppTxBuffer);





void srsIRQHandler(void) {
    uint32_t status = i2c0->hw->intr_stat;

    if (status & I2C_IC_INTR_STAT_R_RX_FULL_BITS) {
        // A received byte is available reading clears irq
        uint32_t cmd = i2c0->hw->data_cmd;
        if (cmd & I2C_IC_DATA_CMD_FIRST_DATA_BYTE_BITS) {
            // Its a 'first byte' -> start rx buffer 
            RxIndex = 0;
            RxBuffer[RxIndex++] = (uint8_t)(cmd & I2C_IC_DATA_CMD_DAT_BITS);
            RxExpectedLen = SrscmdLength(RxBuffer[0]);
            if (RxExpectedLen==0) {
                    // Unknown Command Error
                    ErrorBits |= ERROR_UNKNOWN_CMD;
                    pTxBuffer = UnknownErrorBuffer;
                    TxToSend = 3;
            } else {
                RxCrc = RxBuffer[0];
            }
        } else {
            // Any other byte
            RxBuffer[RxIndex] = (uint8_t)(cmd & I2C_IC_DATA_CMD_DAT_BITS);
            if (RxBuffer[0] == SRS_CTRLCMD_REQDATA) {
                if (RxIndex == 1) {
                    // correct expected length for subcommands - all other than init are 2 byte commands
                    if (  RxBuffer[1] != SRS_DATACMD_INIT ) {
                        RxExpectedLen = 2;
                    }
                }
            }
            if ((RxExpectedLen>0) && (RxIndex >= RxExpectedLen)) {
                // This byte is CRC
                if (RxCrc == RxBuffer[RxIndex]) {
                    memcpy(CmdBuffer, RxBuffer, RxIndex);
                    // CRC OK - resend command in answer... (Always !? -> check with cmds)
                    //pTxBuffer = RxBuffer;
                    //TxToSend = RxIndex + 1;
                    TxToSend = GenerateAnswerIrq(CmdBuffer, RxIndex, &pTxBuffer);
                    
                    // Copy cmd data to (begin) execute in next main loop.
                    if (CmdReceived) {
                        // TODO: Error Cmd overflow
                        ErrorBits |= ERROR_CMD_OVERFLOW;
                    } else {
                        
                        CmdLen = RxIndex;
                        CmdReceived = true;
                    }

                } else {
                    // CRC Error
                    ErrorBits |= ERROR_CRC;
                    pTxBuffer = CrcErrorBuffer;
                    TxToSend = 3;
                }
            } else {
                // another normal data byte
                RxCrc ^= RxBuffer[RxIndex];
                RxIndex++;
            }
        }
    }
  
    if (status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) {
        // A read is requested - write is over now.
        if (TxToSend>0) {
            i2c0->hw->data_cmd = (uint32_t)(*pTxBuffer);
            i2c0->hw->clr_rd_req;
            TxToSend--;
            pTxBuffer++;
        } else {
            i2c0->hw->data_cmd = (uint32_t)0x42;
            i2c0->hw->clr_rd_req;
        }
    }
}


// Module API
void srsInit(void) {
    i2c_init(i2c0, 100000); 
    i2c_set_slave_mode(i2c0, true, SRS_CTRL_ADR);
  
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);     // Pin 5
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);     // Pin 4
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);

    // Enable IRQs for i2c0
    irq_set_exclusive_handler(I2C0_IRQ, srsIRQHandler);
    i2c0->hw->intr_mask = (I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);
    irq_set_enabled(I2C0_IRQ, true);

    for(int i=0;i<12;i++) {
        commonStatus[i] = 0x20 + i;
    }

    for(int i=0;i<30;i++) {
        extendedStatus[i] = 0x80 + i;
    }

} 


void srsMain(void) {
    if (CmdReceived) {
        CmdReceived = false;
        switch(CmdBuffer[0]) {
            case SRS_CTRLCMD_SETSYNCTIME:
                posixTim64 = 0;
                for (int i=0; i<8; i++) {
                    *((uint8_t*)(&posixTim64) + i) = CmdBuffer[i+1];
                }
                printf("Time Sync received: %"PRId64 " \n", posixTim64);
                break;
            case SRS_CTRLCMD_REQSYNCTIME:
                printf("Get Time received answered with: %"PRId64 " \n", posixTim64);
                break;
            case SRS_CTRLCMD_REQSTATUS:
                printf("Request Status answered with byte array.\n");
                break;

            case SRS_CTRLCMD_REQINTERVALS:
                printf("Request Intervals answered with %d %d %d \n", intervalFgDos, intervalRadFet, intervalSram);
                break;

            case SRS_CTRLCMD_SETINTERVALS: {
                uint32_t *target = 0;
                if (CmdBuffer[1] == SRS_STATUSTYPE_FGDOS) {
                    target = &intervalFgDos;
                } else if (CmdBuffer[1] == SRS_STATUSTYPE_RADFET) {
                    target = &intervalRadFet;
                } else if (CmdBuffer[1] == SRS_STATUSTYPE_SRAM) {
                    target = &intervalSram;
                } 
                *target = *((uint32_t*)(&CmdBuffer[2]));
                printf("Interval type %d set to %d \n", CmdBuffer[1], *target);
            }   
            break;

            case SRS_CTRLCMD_SHUTDOWN:
                printf("SHUTDOWN was received!\n");
                break;
    
            case SRS_CTRLCMD_REQDATAADDR:
                printf("REQDATAADDR was received!\n");
                break;
            
            case SRS_CTRLCMD_REQDATA:
                printf("REQDATA subcmd: %d received!\n", CmdBuffer[1]);
                break;


            default:
                printf("Unknown command received! \n");
                break;                    
        }

    }

    if (ErrorBits > 0) {
        if (ErrorBits & ERROR_CRC) {
            ErrorBits &= (~ERROR_CRC);
            printf("CRC Error\n");
        } else if (ErrorBits & ERROR_UNKNOWN_CMD) {
            ErrorBits &= (~ERROR_UNKNOWN_CMD);
            printf("Error: Unknown Command\n");
        } else if (ErrorBits & ERROR_PARAM) {
            ErrorBits &= (~ERROR_PARAM);
            printf("Parameter Error\n");
        } else if (ErrorBits & ERROR_CMD_OVERFLOW) {
            ErrorBits &= (~ERROR_CMD_OVERFLOW);
            printf("Error: Command overflow\n");
        }    
    }
}



uint16_t SrscmdLength(uint8_t b) {
    switch(b) {
        case SRS_CTRLCMD_SETSYNCTIME:
            return 9;
        case SRS_CTRLCMD_REQSYNCTIME:
            return 1;
    	case SRS_CTRLCMD_REQSTATUS:
            return 2;
        case SRS_CTRLCMD_REQINTERVALS:
            return 1;
        case SRS_CTRLCMD_SHUTDOWN:
            return 1;
        case SRS_CTRLCMD_SETINTERVALS:
            return 6;
        case SRS_CTRLCMD_REQDATAADDR:
            return 1;
        case SRS_CTRLCMD_REQDATA:
            return 12;      // Only for subcommand 0x00 - init => else it is 3! for resend/transfer or stop 



        default:
            return 0;
    }
}

uint16_t GenerateAnswerIrq(uint8_t *pCmdBuffer,uint16_t len, uint8_t **ppTxBuffer) {
    int i = 0;
    switch  (pCmdBuffer[0]) {
        uint8_t crc;
        case SRS_CTRLCMD_SETSYNCTIME:
            *ppTxBuffer = RxBuffer;
            return 10;
        case SRS_CTRLCMD_REQSYNCTIME:
            TxBuffer[0] = SRS_CTRLCMD_REQSYNCTIME;
            crc=SRS_CTRLCMD_REQSYNCTIME;
            for (int i=0; i<8; i++) {
                TxBuffer[i+1] = *((uint8_t*)(&posixTim64) + i);
                crc ^= TxBuffer[i+1];
            }
            TxBuffer[9] = crc;
            *ppTxBuffer = TxBuffer;
            return 10;
        case SRS_CTRLCMD_REQSTATUS:
            TxBuffer[i++] = SRS_CTRLCMD_REQSTATUS;
            //TxBuffer[i++] = RxBuffer[1]; 
            if (RxBuffer[1] == SRS_STATUSTYPE_COMMON) {
                TxBuffer[i++] = SRS_STATUSTYPE_COMMON; 
                memcpy(&TxBuffer[i], commonStatus, 12);
                i += 12;
            } else if (RxBuffer[1] == SRS_STATUSTYPE_EXTENDED) {
                TxBuffer[i++] = SRS_STATUSTYPE_EXTENDED; 
                memcpy(&TxBuffer[i], extendedStatus, 30);
                i += 30;
            }  else if (RxBuffer[1] == SRS_STATUSTYPE_BOTH) {
                TxBuffer[i++] = SRS_STATUSTYPE_BOTH; 
                memcpy(&TxBuffer[i], commonStatus, 12);
                i += 12;
                memcpy(&TxBuffer[i], extendedStatus, 30);
                i += 30;
            } else {
                TxBuffer[i++] = SRS_STATUSTYPE_INVALID; 
            }
            TxBuffer[i++] = srsCrc(TxBuffer, i);
            *ppTxBuffer = TxBuffer;
            return i;

    case SRS_CTRLCMD_REQINTERVALS:
            i=0;
            TxBuffer[i++] = SRS_CTRLCMD_REQINTERVALS;
            memcpy(&TxBuffer[i], &intervalFgDos, 4);
            i += 4;
            memcpy(&TxBuffer[i], &intervalRadFet, 4);
            i += 4;
            memcpy(&TxBuffer[i], &intervalSram, 4);
            i += 4;
            TxBuffer[i++] = srsCrc(TxBuffer, i);
            *ppTxBuffer = TxBuffer;
            return i;

    case SRS_CTRLCMD_SETINTERVALS:
            *ppTxBuffer = RxBuffer;
            return 7;

    case SRS_CTRLCMD_REQDATAADDR:
            i=0;
            TxBuffer[i++] = SRS_CTRLCMD_REQDATAADDR;
            TxBuffer[i++] = 0x00;       //Status!?
            memcpy(&TxBuffer[i], &dataStartAddress, 4);
            i += 4;
            memcpy(&TxBuffer[i], &dataEndAddress, 4);
            i += 4;
            TxBuffer[i++] = srsCrc(TxBuffer, i);
            *ppTxBuffer = TxBuffer;
            return i;

    case SRS_CTRLCMD_REQDATA:
            if (pCmdBuffer[1] == SRS_DATACMD_INIT) {
                return answerInitData(ppTxBuffer);
            } else if ((pCmdBuffer[1] == SRS_DATACMD_TRANSFER)) {
                return answerNextDataBlock(ppTxBuffer);
            } else if ((pCmdBuffer[1] == SRS_DATACMD_RESEND)) {
                return answerCurrentDataBlock(ppTxBuffer);
            } else if ((pCmdBuffer[1] == SRS_DATACMD_STOP)) {
                return answerStop(ppTxBuffer);
            } else {
                // Unknnown (Sub)Command
                return 0;
            }

    default:
            return 0;

    }
}


int answerInitData(uint8_t **ppTxBuffer) {
    *ppTxBuffer = RxBuffer;
    memcpy(&blocksToSend, &RxBuffer[8], 4);
    memcpy(&blocksRequested, &RxBuffer[8], 4);

    //memcpy(&dataStartAddress, &RxBuffer[4], 4);

    memcpy(&blockSize, &RxBuffer[2], 2);
    return 13;
}

int answerCurrentDataBlock(uint8_t **ppTxBuffer) {
    blocksToSend++;
    return answerNextDataBlock(ppTxBuffer);
}



int answerNextDataBlock(uint8_t **ppTxBuffer) {
    int i=0;
    uint32_t currBlockNr;
    uint32_t blockStart;
    uint32_t blockEnd; 
    if (blocksToSend>0) {
        uint16_t bytesToSend = blockSize;
        currBlockNr = (blocksRequested - blocksToSend);
        blockStart = ((blocksRequested - blocksToSend) * blockSize) + dataStartAddress;
        blockEnd = blockStart + blockSize -1;
        if (blockEnd > dataEndAddress) {
            blockEnd = dataEndAddress;
            bytesToSend = (dataEndAddress - blockStart + 1);
        }

        blocksToSend--;
        TxBuffer[i++] = SRS_CTRLCMD_REQDATA;
        TxBuffer[i++] = SRS_DATACMD_TRANSFER;  
        memcpy(&TxBuffer[i], &blockStart, 4);
        i += 4;
        memcpy(&TxBuffer[i], &blockEnd, 4);
        i += 4;
        memcpy(&TxBuffer[i], &currBlockNr, 4);
        i += 4;
        memcpy(&TxBuffer[i], &bytesToSend, 2);
        i += 2;
        for (int j=0;j<bytesToSend; j++) {
            TxBuffer[i+j] = (uint8_t)j;
        }
        i += bytesToSend;
    } else {
        TxBuffer[i++] = SRS_CTRLCMD_REQDATA;
        TxBuffer[i++] = SRS_DATARESP_NOMMOREDATA;  
    }
    TxBuffer[i++] = srsCrc(TxBuffer, i);
    *ppTxBuffer = TxBuffer;
    return i;
}

int answerStop(uint8_t **ppTxBuffer) {
    int i=0;
    TxBuffer[i++] = SRS_CTRLCMD_REQDATA;
    TxBuffer[i++] = SRS_DATACMD_STOP;  
    TxBuffer[i++] = srsCrc(TxBuffer, i);
    *ppTxBuffer = TxBuffer;
    return i;
}





uint8_t srsCrc(uint8_t *data, uint16_t len) {
    uint8_t ret = 0x00;
	while (len>0) {
		ret ^= data[--len];
	}
    answerCnt++;
    if (crcErrorEvery>0) {
        if ((answerCnt%crcErrorEvery) == 0) {
            ret++;
        }
    }
	return ret;
}


// Module Commands
void srsShow(int argc, char* argv[]) {
    if (argc != 2) {
		puts("uasge: show <adr> <len>\n");
	} else {
		// CLI params to binary params
		uint32_t adr = atoi(argv[0]);
		uint32_t len = atoi(argv[1]);
        printf("\n");
		for (int i=adr;i<adr+len;i++){
            printf("%02x ", RxBuffer[i]);
            if ((i+1-adr)%16 == 0) {
                printf("\n");
            } 
		}
        printf("\n");
	}
}


void srsSetDataArea(int argc, char* argv[]) {
    if (argc > 0) {
        dataStartAddress = atoi(argv[0]);
    }
    if (argc > 1) {
        dataEndAddress = atoi(argv[1]);
    }
    if (dataEndAddress<dataStartAddress) {
        dataEndAddress = dataEndAddress + 10;
    }
    printf("Data Area set to Start: %d End: %d\n", dataStartAddress, dataEndAddress);
}

void srsSetCrcSim(int argc, char* argv[]) {
    crcErrorEvery = 0;
    if (argc > 0) {
        crcErrorEvery = atoi(argv[0]);
    }
    printf("SRS Sim will create a CRC error every %d answer.\n", crcErrorEvery);
}


