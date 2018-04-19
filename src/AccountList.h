#include <IPAddress.h>
#include <Adafruit_SSD1306.h>
struct node
{
  IPAddress address;
  String mail;
  String password;
  node *next;
  node *previous;
};
class AccountList{
private:
  node *currentLocation, *lastNode, *firstNode;
public:
  AccountList();
  bool addNode(String mail, String password, IPAddress address);
  void moveUp();
  void moveDown();
  void print(Adafruit_SSD1306 *display);
};
