#include "WinbondW25N.h"

W25N flash = W25N();

char* myString = "Hello world from the W25N flash chip!";
char buf[512];



void setup(){
  SerialUSB.begin(1000000);
  while(!SerialUSB){};
  delay(1000);

  //Make sure the buffer is zero'd
  zeroBuffer(buf, sizeof(buf));
  
  if(flash.begin(2) == 0){
    SerialUSB.println("Flash init successful");
  } else {
    SerialUSB.println("Flash init Failed");
  }

  //put the strig into our flash buffer
  //We cant point to the string directly as it is static
  //and the buffer will be modified with the recieved data
  memcpy(buf, myString, strlen(myString) + 1);
  //erase the flash page we will be writing to
  flash.blockErase(0);

  //Transfer the data we want to program and execute the program command
  flash.loadProgData(0, buf, strlen(myString) + 1);
  flash.ProgramExecute(0);

  //zero the buffer again just so we know that the data we print will be from the flash
  zeroBuffer(buf, sizeof(buf));
  
  //read the data back and print in it
  flash.pageDataRead(0);
  flash.read(0, buf, sizeof(buf));

  //Print the data read from the flash chip
  SerialUSB.println(buf);
}

void loop(){

}

void zeroBuffer(char* buf, uint32_t buflen){
  for(int i = 0; i < buflen; i++){
    buf[i] = 0;
  }
}
