/*
 * Winbond W25N Flash Library
 * Written by Cameron Houston for UGR
 * 09 2019
 */
 
#ifndef WinbondW25N_H
#define WinbondW25N_H
 
#include "Arduino.h"
#include <SPI.h>

#define W25N_RESET					0xFF
#define W25N_JEDEC_ID				0x9F
#define W25N_READ_STATUS_REG		0x05
#define W25N_WRITE_STATUS_REG		0x01
#define W25N_WRITE_ENABLE			0x06
#define W25N_WRITE_DISABLE			0x04
#define W25N_BB_MANAGE				0xA1
#define W25N_READ_BBM				0xA5
#define W25N_LAST_ECC_FAIL			0xA9
#define W25N_BLOCK_ERASE			0xD8
#define W25N_PROG_DATA_LOAD			0x02
#define W25N_RAND_PROG_DATA_LOAD	0x84
#define W25N_PROG_EXECUTE			0x10
#define W25N_PAGE_DATA_READ			0x13
#define W25N_READ					0x03
#define W25N_FAST_READ				0x0B

#define W25N_PROT_REG				0xA0
#define W25N_CONFIG_REG				0xB0
#define W25N_STAT_REG				0xC0

#define W25N_JEDEC_RETURN_1			0xEF
#define W25N_JEDEC_RETURN_2			0xAA
#define W25N_JEDEC_RETURN_3			0x21

#define W25N_MAX_PAGE 65535


class W25N {
	private:
		int _cs;
	public:
		W25N();
		void sendData(char * buf, int len);
		int begin(int cs);
		void reset();
		char getStatusReg(char reg);
		void setStatusReg(char reg, char set);
		void writeEnable();
		void writeDisable();
		void blockErase(uint16_t pageAdd);
		void bulkErase();
		void loadProgData(uint16_t columnAdd, char* buf, uint32_t dataLen);
		void loadRandProgData(uint16_t columnAdd, char* buf, uint32_t dataLen);
		void ProgramExecute(uint16_t add);
		void pageDataRead(uint16_t add);
		void read(uint16_t columnAdd, char* buf, uint32_t dataLen);
		int check_WIP();                                           //Returns the Write In Progress bit from flash.
		void block_WIP();
		int check_status();
		
};

#endif