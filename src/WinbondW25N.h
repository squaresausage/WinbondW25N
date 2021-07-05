/*
 * Winbond W25N Flash Library
 * Written by Cameron Houston for UGRacing Formula Student
 * 09 2019
 */

//TODO add support for multi-Gb chips that require bank switching
//TODO add proper error codes
//TODO add ECC support functions
 
#ifndef WinbondW25N_H
#define WinbondW25N_H
 
#include "Arduino.h"
#include <SPI.h>

#define W25M_DIE_SELECT           0xC2

#define W25N_RESET                0xFF
#define W25N_JEDEC_ID             0x9F
#define W25N_READ_STATUS_REG      0x05
#define W25N_WRITE_STATUS_REG     0x01
#define W25N_WRITE_ENABLE         0x06
#define W25N_WRITE_DISABLE        0x04
#define W25N_BB_MANAGE            0xA1
#define W25N_READ_BBM             0xA5
#define W25N_LAST_ECC_FAIL        0xA9
#define W25N_BLOCK_ERASE          0xD8
#define W25N_PROG_DATA_LOAD       0x02
#define W25N_RAND_PROG_DATA_LOAD  0x84
#define W25N_PROG_EXECUTE         0x10
#define W25N_PAGE_DATA_READ       0x13
#define W25N_READ                 0x03
#define W25N_FAST_READ            0x0B

#define W25N_PROT_REG             0xA0
#define W25N_CONFIG_REG           0xB0
#define W25N_STAT_REG             0xC0

#define WINBOND_MAN_ID            0xEF
#define W25N01GV_DEV_ID           0xAA21

#define W25M02GV_DEV_ID           0xAB21

#define W25N01GV_MAX_PAGE         65535
#define W25N_MAX_COLUMN           2112
#define W25M02GV_MAX_PAGE         131071
#define W25M02GV_MAX_DIES         2

enum chipModels{
  W25N01GV,
  W25M02GV
};


class W25N {
  private:
    int _cs;
    enum chipModels _model;
    int _dieSelect;

    /* int dieSelect(uint32_t die) -- Selects the active die on a multi die chip (W25*M*)
     * Input - die number starting at 0
     * Output - error output, 0 for success
     */
    int dieSelect(char die);


  public:
    W25N();

    /* sendData(char * buf, int len) -- Sends/recieves data to the flash chip.
     * The buffer that is passed to the function will have its dat sent to the
     * flash chip, and the data recieved will be written back to that same 
     * buffer. */
    void sendData(char * buf, uint32_t len);

    /* begin(int cs) -- initialises the flash and checks that the flash is 
     * functioning and is the right model.
     * Output -- 0 if working, 1 if error*/
    int begin(uint32_t cs);

    /* reset() -- resets the device. */
    void reset();

    /* int dieSelectOnAdd(uint32_t pageAdd) -- auto changes selected die based on requested address
     * Input - full range (across all dies) page address
     * Output - error output, 0 for success
     */
    int dieSelectOnAdd(uint32_t pageAdd);

    /* getStatusReg(char reg) -- gets the char value from one of the registers:
     * W25N_STAT_REG / W25N_CONFIG_REG / W25N_PROT_REG
     * Output -- register byte value
     */
    char getStatusReg(char reg);

    /* setStatusReg(char reg, char set) -- Sets one of the status registers:
     * W25N_STAT_REG / W25N_CONFIG_REG / W25N_PROT_REG
     * set input -- char input to set the reg to */
    void setStatusReg(char reg, char set);

    /* getMaxPage() Returns the max page for the given chip
     */
    uint32_t getMaxPage();

    /* writeEnable() -- enables write opperations on the chip.
     * Is disabled after a write operation and must be recalled.
     */
    void writeEnable();

    /* writeDisable() -- disables all write opperations on the chip */
    void writeDisable();

    /* blockErase(uint32_t pageAdd) -- Erases one block of data on the flash chip. One block is 64 Pages, and any given 
     * page address within the block will erase that block.
     * Rerturns 0 if successful
     * */
    int blockErase(uint32_t pageAdd);

    /* bulkErase() -- Erases the entire chip
     * THIS TAKES A VERY LONG TIME, ~30 SECONDS
     * Returns 0 if successful 
     * */
    int bulkErase();

    /* loadRandProgData(uint16_t columnAdd, char* buf, uint32_t dataLen) -- Transfers datalen number of bytes from the 
     * given buffer to the internal flash buffer, to be programed once a ProgramExecute command is sent.
     * datalLen cannot be more than the internal buffer size of 2111 bytes, or 2048 if ECC is enabled on chip.
     * When called any data in the internal buffer beforehand will be nullified.
     * WILL ERASE THE DATA IN BUF OF LENGTH DATALEN BYTES
     * */
    int loadProgData(uint16_t columnAdd, char* buf, uint32_t dataLen);
    int loadProgData(uint16_t columnAdd, char* buf, uint32_t dataLen, uint32_t pageAdd);

    /* loadRandProgData(uint16_t columnAdd, char* buf, uint32_t dataLen) -- Transfers datalen number of bytes from the 
     * given buffer to the internal flash buffer, to be programed once a ProgramExecute command is sent.
     * datalLen cannot be more than the internal buffer size of 2111 bytes, or 2048 if ECC is enabled on chip.
     * Unlike the normal loadProgData the loadRandProgData function allows multiple transfers to the internal buffer
     * without the nulling of the currently kept data. 
     * WILL ERASE THE DATA IN BUF OF LENGTH DATALEN BYTES
     */
    int loadRandProgData(uint16_t columnAdd, char* buf, uint32_t dataLen);
    int loadRandProgData(uint16_t columnAdd, char* buf, uint32_t dataLen, uint32_t pageAdd);

    /* ProgramExecute(uint32_t add) -- Commands the flash to program the internal buffer contents to the addres page
     * given after a loadProgData or loadRandProgData has been called.
     * The selected page needs to be erased prior to use as the falsh chip can only change 1's to 0's
     * This command will put the flash in a busy state for a time, so busy checking is required ater use.  */
    int ProgramExecute(uint32_t pageAdd);

    //pageDataRead(uint32_t add) -- Commands the flash to read from the given page address into
    //its internal buffer, to be read using the read() function. 
    //This command will put the flash in a busy state for a time, so busy checking is required after use.
    int pageDataRead(uint32_t pageAdd);

    //read(uint16_t columnAdd, char* buf, uint32_t dataLen) -- Reads data from the flash internal buffer
    //columnAdd is a buffer index (0-2047) or (0 - 2111) including ECC bits
    //char* buf is a pointer to the buffer to be read into
    //datalen is the length of data that should be read from the buffer (up to 2111)
    int read(uint16_t columnAdd, char* buf, uint32_t dataLen);

    //check_WIP() -- checks if the flash is busy with an operation
    //Output: true if busy, false if free
    int check_WIP();

    //block_WIP() -- checks if the flash is busy and only returns once free
    //Has a 15ms timeout
    int block_WIP();


    int check_status();
    
};

#endif