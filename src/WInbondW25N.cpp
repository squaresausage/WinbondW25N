#include "WinbondW25N.h"

W25N::W25N(){};


void W25N::sendData(char * buf, int len){
	digitalWrite(_cs, LOW);
	SPI.transfer(buf, len);
	digitalWrite(_cs, HIGH);
}

int W25N::begin(int cs){
	SPI.begin();
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  _cs = cs;
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);

  this->reset();

  char jedec[5] = {W25N_JEDEC_ID, 0x00, 0x00, 0x00, 0x00,};
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

void W25N::blockErase(uint16_t pageAdd){
  char pageHigh = (char)((pageAdd & 0xFF00) >> 8);
  char pageLow = (char)(pageAdd);
  char buf[4] = {W25N_BLOCK_ERASE, 0x00, pageHigh, pageLow};
  this->block_WIP();
  this->writeEnable();
  this->sendData(buf, sizeof(buf));
}

void W25N::bulkErase(){
for(int i = 0; i < W25N_MAX_PAGE; i++){
  this->blockErase(i);
}
}

void W25N::loadProgData(uint16_t columnAdd, char* buf, uint32_t dataLen){
  char columnHigh = (columnAdd & 0xFF00) >> 8;
  char columnLow = columnAdd & 0xff;
  char cmdbuf[3] = {W25N_PROG_DATA_LOAD, columnHigh, columnLow};
  this->block_WIP();
  this->writeEnable();
	digitalWrite(_cs, LOW);
	SPI.transfer(cmdbuf, sizeof(cmdbuf));
  //TODO check data len
  SPI.transfer(buf, dataLen);
	digitalWrite(_cs, HIGH);
}

void W25N::loadRandProgData(uint16_t columnAdd, char* buf, uint32_t dataLen){
  char columnHigh = (columnAdd & 0xFF00) >> 8;
  char columnLow = columnAdd & 0xff;
  char cmdbuf[3] = {W25N_RAND_PROG_DATA_LOAD, columnHigh, columnLow};
  this->block_WIP();
  this->writeEnable();
	digitalWrite(_cs, LOW);
	SPI.transfer(cmdbuf, sizeof(cmdbuf));
  //TODO check data len
  SPI.transfer(buf, dataLen);
	digitalWrite(_cs, HIGH);
}

void W25N::ProgramExecute(uint16_t add){
  char pageHigh = (char)((add & 0xFF00) >> 8);
  char pageLow = (char)(add);
  this->writeEnable();
  char buf[4] = {W25N_PROG_EXECUTE, 0x00, pageHigh, pageLow};
  this->sendData(buf, sizeof(buf));
}

void W25N::pageDataRead(uint16_t add){
  char pageHigh = (char)((add & 0xFF00) >> 8);
  char pageLow = (char)(add);
  char buf[4] = {W25N_PAGE_DATA_READ, 0x00, pageHigh, pageLow};
  this->block_WIP();
  this->sendData(buf, sizeof(buf));

}

void W25N::read(uint16_t columnAdd, char* buf, uint32_t dataLen){
  char columnHigh = (columnAdd & 0xFF00) >> 8;
  char columnLow = columnAdd & 0xff;
  char cmdbuf[4] = {W25N_READ, columnHigh, columnLow, 0x00};
  this->block_WIP();
	digitalWrite(_cs, LOW);
	SPI.transfer(cmdbuf, sizeof(cmdbuf));
  //TODO check data len
  SPI.transfer(buf, dataLen);
	digitalWrite(_cs, HIGH);
}
//Returns the Write In Progress bit from flash.
int W25N::check_WIP(){
  char status = this->getStatusReg(W25N_STAT_REG);
  if(status & 0x01){
    return 1;
  }
  return 0;
}                      

void W25N::block_WIP(){
  //TODO Add timeout? 
  while(this->check_WIP()){
    delay(10);
  };
}

int W25N::check_status(){
  return(this->getStatusReg(W25N_STAT_REG));
}