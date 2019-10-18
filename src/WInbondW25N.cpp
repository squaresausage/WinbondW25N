#include "WinbondW25N.h"

W25N::W25N(){};


void W25N::sendData(char * buf, uint32_t len){
  SPI.beginTransaction(SPISettings(80000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(buf, len);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

int W25N::begin(uint32_t cs){
  SPI.begin();
  _cs = cs;
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);

  this->reset();

  char jedec[5] = {W25N_JEDEC_ID, 0x00, 0x00, 0x00, 0x00};
  this->sendData(jedec, sizeof(jedec));
  if(jedec[2] == W25N_JEDEC_RETURN_1){
    if(jedec[3] == W25N_JEDEC_RETURN_2 && jedec[4] == W25N_JEDEC_RETURN_3){
      this->setStatusReg(W25N_PROT_REG, 0x00);
      return 0;
    }
  }
  return 1;
}

void W25N::reset(){
  char buf[] = {W25N_RESET};
  this->sendData(buf, sizeof(buf));
}

char W25N::getStatusReg(char reg){
  char buf[3] = {W25N_READ_STATUS_REG, reg, 0x00};
  this->sendData(buf, sizeof(buf));
  return buf[2];
}

void W25N::setStatusReg(char reg, char set){
  char buf[3] = {W25N_WRITE_STATUS_REG, reg, set};
  this->sendData(buf, sizeof(buf));
}

void W25N::writeEnable(){
  char buf[] = {W25N_WRITE_ENABLE};
  this->sendData(buf, sizeof(buf));
}

void W25N::writeDisable(){
  char buf[] = {W25N_WRITE_DISABLE};
  this->sendData(buf, sizeof(buf));
}

int W25N::blockErase(uint32_t pageAdd){
  if(pageAdd > W25N_MAX_PAGE) return 1;
  char pageHigh = (char)((pageAdd & 0xFF00) >> 8);
  char pageLow = (char)(pageAdd);
  char buf[4] = {W25N_BLOCK_ERASE, 0x00, pageHigh, pageLow};
  this->block_WIP();
  this->writeEnable();
  this->sendData(buf, sizeof(buf));
  return 0;
}

int W25N::bulkErase(){
  int error = 0;
  for(int i = 0; i < W25N_MAX_PAGE; i++){
    if((error = this->blockErase(i)) != 0) return error;
  }
  return 0;
}

int W25N::loadProgData(uint16_t columnAdd, char* buf, uint32_t dataLen){
  if(columnAdd > W25N_MAX_COLUMN) return 1;
  if(dataLen > W25N_MAX_COLUMN - columnAdd) return 1;
  char columnHigh = (columnAdd & 0xFF00) >> 8;
  char columnLow = columnAdd & 0xff;
  char cmdbuf[3] = {W25N_PROG_DATA_LOAD, columnHigh, columnLow};
  this->block_WIP();
  this->writeEnable();
  SPI.beginTransaction(SPISettings(80000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(cmdbuf, sizeof(cmdbuf));
  SPI.transfer(buf, dataLen);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  return 0;
}

int W25N::loadRandProgData(uint16_t columnAdd, char* buf, uint32_t dataLen){
  if(columnAdd > W25N_MAX_COLUMN) return 1;
  if(dataLen > W25N_MAX_COLUMN - columnAdd) return 1;
  char columnHigh = (columnAdd & 0xFF00) >> 8;
  char columnLow = columnAdd & 0xff;
  char cmdbuf[3] = {W25N_RAND_PROG_DATA_LOAD, columnHigh, columnLow};
  this->block_WIP();
  this->writeEnable();
  SPI.beginTransaction(SPISettings(80000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(cmdbuf, sizeof(cmdbuf));
  SPI.transfer(buf, dataLen);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  return 0;
}

int W25N::ProgramExecute(uint32_t pageAdd){
  if(pageAdd > W25N_MAX_PAGE) return 1;
  char pageHigh = (char)((pageAdd & 0xFF00) >> 8);
  char pageLow = (char)(pageAdd);
  this->writeEnable();
  char buf[4] = {W25N_PROG_EXECUTE, 0x00, pageHigh, pageLow};
  this->sendData(buf, sizeof(buf));
  return 0;
}

int W25N::pageDataRead(uint32_t pageAdd){
  if(pageAdd > W25N_MAX_PAGE) return 1;
  char pageHigh = (char)((pageAdd & 0xFF00) >> 8);
  char pageLow = (char)(pageAdd);
  char buf[4] = {W25N_PAGE_DATA_READ, 0x00, pageHigh, pageLow};
  this->block_WIP();
  this->sendData(buf, sizeof(buf));
  return 0;

}

int W25N::read(uint16_t columnAdd, char* buf, uint32_t dataLen){
  if(columnAdd > W25N_MAX_COLUMN) return 1;
  if(dataLen > W25N_MAX_COLUMN - columnAdd) return 1;
  char columnHigh = (columnAdd & 0xFF00) >> 8;
  char columnLow = columnAdd & 0xff;
  char cmdbuf[4] = {W25N_READ, columnHigh, columnLow, 0x00};
  this->block_WIP();
  SPI.beginTransaction(SPISettings(80000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(cmdbuf, sizeof(cmdbuf));
  SPI.transfer(buf, dataLen);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  return 0;
}
//Returns the Write In Progress bit from flash.
int W25N::check_WIP(){
  char status = this->getStatusReg(W25N_STAT_REG);
  if(status & 0x01){
    return 1;
  }
  return 0;
}                      

int W25N::block_WIP(){
  //Max WIP time is 10ms for block erase so 15 should be a max.
  long tstamp = millis();
  while(this->check_WIP()){
    delay(1);
    if (millis() > tstamp + 15) return 1;
  }
  return 0;
}

int W25N::check_status(){
  return(this->getStatusReg(W25N_STAT_REG));
}