#include <WiFi.h>
#include <WiFiUdp.h>
//#include <ESP8266WiFi.h>
//#include <ESP8266WebServerSecure.h>

#undef BIT1
#undef BIT2
#undef BIT3
#undef BIT4
#undef BIT5
#undef BIT6
#undef BIT7
#undef BIT8

#define BIT1 0x80
#define BIT2 0x40
#define BIT3 0x20
#define BIT4 0x10
#define BIT5 0x08
#define BIT6 0x04
#define BIT7 0x02
#define BIT8 0x01

#define POINTER_FLAG 0xc0
#define HEADER_SIZE 12 // in bytes

typedef struct Packet{
char buffer[512];
int size=0;
} Packet;





class DNSMessage{
public:
  DNSMessage(Packet packet);
  bool creatResponseDNSPacket(IPAddress myIP);
  Packet message;
  Packet response;

  char *domain;
  int nameLength = 0;
  int questionSize = 0;



private:
  void calcResponseSize();

};
