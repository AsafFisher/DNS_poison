#include "AccountList.h"
#include "Arduino.h"

  AccountList::AccountList(){
    currentLocation = NULL;
  }
  bool AccountList::addNode(String mail, String password, IPAddress address){
    node *temp = new node;
    if(mail.length()>=100 || password.length()>=100){
      return false;
    }
    temp->mail = mail;
    temp->password = password;
    temp->address = address;
    if(this->currentLocation==NULL){
      firstNode = temp;
      currentLocation = temp;
      lastNode = temp;

    }else{
      lastNode->next = temp;
      temp->previous = lastNode;
      lastNode = temp;
    }

    return true;
  }
  void AccountList::moveUp(){
    if(this->currentLocation == NULL){
      return;
    }
    this->currentLocation = this->currentLocation->previous!=NULL ? this->currentLocation->previous : this->currentLocation;
  }
  void AccountList::moveDown(){
    if(this->currentLocation == NULL){
      return;
    }
      this->currentLocation = this->currentLocation->next != NULL ? this->currentLocation->next : this->firstNode;
  }
  void AccountList::print(Adafruit_SSD1306 *display){
    int i = 0;
    if(this->currentLocation == NULL){
      return;
    }
    Serial.println("A");
    node *temp = currentLocation;
    display->clearDisplay();
//    display->setTextSize(1);
    display->setCursor(0,0);
    display->setTextColor(WHITE);
    Serial.println("B");
    display->println("Account Information");
    Serial.println("C");
    display->println("-------------------");
    Serial.println("D");
    //display->println("IP address: ");
    //display->println(this->currentLocation->address.toString());
    display->println("Email address: ");
    Serial.println("E");
    display->println(this->currentLocation->mail);
    Serial.println("F");
    display->println("Password: ");
    display->println(this->currentLocation->password);
    Serial.println("G");
    display->println("-------------------");

    display->display();
    Serial.println("D");



  }
