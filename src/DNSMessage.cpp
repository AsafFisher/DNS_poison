#include "DNSMessage.h"

DNSMessage :: DNSMessage(Packet packet){
  this->message = packet;
  calcResponseSize();
}

void DNSMessage ::  calcResponseSize(){
  this->response.size = HEADER_SIZE;
  //Serial.println(this->response.size);
  this->domain = this->message.buffer + this->response.size;
  while(this->message.buffer[this->response.size] != 0){
    //Serial.printf("Requested domain: %c\n", this->message[r_size]);
    this->response.size++;
    this->nameLength++;
  }
  //Serial.println(this->response.size);
  // Extra 0x00 in the end.
  this->response.size++;


  this->questionSize = this->nameLength+1; // 0x00 Btye in the end.
  this->questionSize += 4; // 2 byte QTYPE + 2 byte QCLASS
//Size of QTYPE and QCLASS
  this->response.size = this->response.size + 4;

//--------- DNS Answer Size ------------
//pointer name size.
this->response.size += 2;
//TYPE Size
this->response.size += 2;
//CLASS Size
this->response.size += 2;
//TTL Size
this->response.size += 4;
//RDLENGTH Size
this->response.size += 2;
//RDATA Size
this->response.size += 4;


  //Serial.println("TEST: ");
  //Serial.println(this->response.size);
}
bool DNSMessage :: creatResponseDNSPacket(IPAddress myIP){
//Set the correct size of the response buffer.
  if((this->message.buffer[3]&BIT1)){
    this->response.size = 0;
    return false;
  }
//  this->response.size = calcResponseSize();

//Setup response buffer
//----------- HEADER ----------------
  //Message ID
  this->response.buffer[0] = this->message.buffer[0];
  this->response.buffer[1] = this->message.buffer[1];
  //flags:
  //QR and 3 first bits of OPCODE : 1 000
  this->response.buffer[2] = BIT1; // 1000 0000
  this->response.buffer[3] = BIT3;//  0010 0000
  //Q count
  this->response.buffer[4] = 0;
  this->response.buffer[5] = 1;
  //A count
  this->response.buffer[6] = 0;
  this->response.buffer[7] = 1;

  this->response.buffer[8] = 0;
  this->response.buffer[9] = 0;
  this->response.buffer[10] = 0;
  this->response.buffer[11] = 0;
  //-----------------------------

  // ------- COPY NAME -----
  int i = 0;
  while(i<questionSize){
    this->response.buffer[HEADER_SIZE+i] = this->message.buffer[HEADER_SIZE+i];
    i++;
  }

//NAME OFFSET
  this->response.buffer[HEADER_SIZE+i] = POINTER_FLAG|0x00; //1100 0000 offset 12 is the constant offset -> 0x0b
  /*Serial.println("DEBBUG: ");
  Serial.println(POINTER_FLAG|0x00);
  Serial.println(this->response.buffer[HEADER_SIZE+i-1], HEX);
  Serial.println(this->response.buffer[HEADER_SIZE+i],HEX);*/
  i++;
                        //TWO FIRST BITS is flag for pointer using.
  this->response.buffer[HEADER_SIZE+i] = 0x0c;                 //1100 0000 offset 12 is the constant offset -> 0x0b
  i++;


  //TYPE
  this->response.buffer[HEADER_SIZE+i] = 0x00;
  i++;
  this->response.buffer[HEADER_SIZE+i] = 0x01;
  i++;


  //CLASS
  this->response.buffer[HEADER_SIZE+i] = 0x00;
  i++;
  this->response.buffer[HEADER_SIZE+i] = 0x01;
  i++;

  //TTL
  this->response.buffer[HEADER_SIZE+i] = 0x00;
  i++;
  this->response.buffer[HEADER_SIZE+i] = 0xFF;
  i++;
  this->response.buffer[HEADER_SIZE+i] = 0xFF;
  i++;
  this->response.buffer[HEADER_SIZE+i] = 0xFF;

  //RDLENGTH
  i++;
  this->response.buffer[HEADER_SIZE+i] = 0x00;
  i++;
  this->response.buffer[HEADER_SIZE+i] = 0x04;
  i++;
  //DATA
  this->response.buffer[HEADER_SIZE+i] = myIP[0];//xxx.000.000.000
  i++;
  this->response.buffer[HEADER_SIZE+i] = myIP[1];//000.xxx.000.000
  i++;
  this->response.buffer[HEADER_SIZE+i] = myIP[2];//000.000.xxx.000
  i++;
  this->response.buffer[HEADER_SIZE+i] = myIP[3];//000.000.000.xxx
  i++;

  /*Serial.println("----");
Serial.println(i+HEADER_SIZE);
Serial.println(this->response.size);
Serial.println("----");*/
  if(i+HEADER_SIZE==this->response.size){
    //Serial.println("Succsess!!");
    return true;
  }
  return false;
  //response[]

}
