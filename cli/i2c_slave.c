#include <stdio.h>
#include <stdlib.h>

#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include "MyI2cSlave.hpp"

#define ICS_MEMORY_SIZE 256
#define ICS_SLAVE_ADR   0x0B


uint16_t memAddr = 0;
uint8_t Memory[ICS_MEMORY_SIZE];


void icsIRQHandler(void) {
    uint32_t status = i2c0->hw->intr_stat;

    if (status & I2C_IC_INTR_STAT_R_RX_FULL_BITS) {
        // A received byte is available reading clears irq
        uint32_t cmd = i2c0->hw->data_cmd;
        if (cmd & I2C_IC_DATA_CMD_FIRST_DATA_BYTE_BITS) {
            // Its a 'first byte' -> store as index for access
            memAddr = (uint8_t)(cmd & I2C_IC_DATA_CMD_DAT_BITS);
        } else {
            // Any other byte
            Memory[memAddr++] = (uint8_t)(cmd & I2C_IC_DATA_CMD_DAT_BITS);
        }
    }
  
    if (status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) {
        // A read is requested.
        i2c0->hw->data_cmd = (uint32_t)Memory[memAddr++];
        i2c0->hw->clr_rd_req;
    }
}


// Module API
void icsInit(void) {

    i2c_init(i2c0, 100000);
    i2c_set_slave_mode(i2c0, true, ICS_SLAVE_ADR);

  
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);     // Pin 5
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);     // Pin 4
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);

    // Enable IRQs for i2c0
    irq_set_exclusive_handler(I2C0_IRQ, icsIRQHandler);
    i2c0->hw->intr_mask = (I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);
    irq_set_enabled(I2C0_IRQ, true);

    for(int i=0;i<ICS_MEMORY_SIZE;i++) {
        Memory[i]=(uint8_t)i;
    } 
} 


// Module Commands
void icsShow(int argc, char* argv[]) {
    if (argc != 2) {
		puts("uasge: show <adr> <len>\n");
	} else {
		// CLI params to binary params
		uint32_t adr = atoi(argv[0]);
		uint32_t len = atoi(argv[1]);
        printf("\n");
		for (int i=adr;i<adr+len;i++){
            printf("%02x ", Memory[i]);
            if ((i+1-adr)%16 == 0) {
                printf("\n");
            } 
		}
        printf("\n");
	}
}

void icsSet(int argc, char* argv[]) {
    if (argc != 3) {
		puts("uasge: set <adr> <data> <len>\n");
	} else {
		// CLI params to binary params
		uint16_t adr = atoi(argv[0]);
        uint8_t data = atoi(argv[1]);
		uint16_t len = atoi(argv[2]);

        if (adr + len < ICS_MEMORY_SIZE) {
           	for (int i=adr;i<adr+len;i++){
                Memory[i] = data;    
		    }
        }
	}
}

rk_myI2cSlave_t lastCreated = NULL;
rk_myI2cSlave_t allSlaves[10];
int curIdx = 0;

void icsNewSlave(int argc, char* argv[]) {
if (argc != 2) {
		puts("uasge: new <slaveAdr> <dataSize>\n");
	} else {
		// CLI params to binary params
		uint8_t adr = atoi(argv[0]);
        uint16_t size = atoi(argv[1]);

        void* ptr = rk_create_myI2cSlave_with_int(adr, size);
        printf("Ptr[%d]: %08x\n ",curIdx, ptr);

        if (curIdx >= 10) {
            curIdx = 0;
        }
        if (allSlaves[curIdx] != NULL) {
            printf("feree old Ptr[%d]: %08x\n", curIdx, allSlaves[curIdx]);
            rk_free_myI2cSlave(allSlaves[curIdx]);
        }
        allSlaves[curIdx++] = ptr;
        lastCreated = ptr;
	}
}