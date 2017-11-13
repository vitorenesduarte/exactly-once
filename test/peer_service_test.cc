#include <unistd.h>
#include "../include/peer_service.h"

using std::cout;
using std::endl;

class MyWrapper : public Wrapper<int> {
public:
  MyWrapper(int* x) : Wrapper(x) { }

  void apply(char* buf, int len) {
    *t_ += len;
  }
};

void test1() {
  int x0 = 0;
  Wrapper<int>* w0 = new MyWrapper(&x0);
  PeerService<int> ps0;
  ps0.start(3000, w0);
  sleep(1);

  int x1 = 0;
  Wrapper<int>* w1 = new MyWrapper(&x1);
  PeerService<int> ps1;
  ps1.start(3001, w1);
  sleep(1);

  // ps0 joins ps1
  ps0.join(1, "127.0.0.1", 3001);
 
  // ps1 joins ps0
  ps1.join(0, "127.0.0.1", 3000);

  string msg;

  // ps0 sends "HEY" to ps1
  msg = "HEY";
  ps0.send(1, msg.c_str(), msg.size());
  sleep(1);
  assert(x0 == 0);
  assert(x1 == 3);

  // ps0 sends "HI" to ps1
  msg = "HI";
  ps0.send(1, msg.c_str(), msg.size());
  sleep(1);
  assert(x0 == 0);
  assert(x1 == 5);

  // ps1 sends "HOW ARE YOU" to ps0
  msg = "HOW ARE YOU";
  ps1.send(0, msg.c_str(), msg.size());
  sleep(1);
  assert(x0 == 11);
  assert(x1 == 5);

  // ps0 sends "GOOD" to ps1
  msg = "GOOD";
  ps0.send(1, msg.c_str(), msg.size());
  sleep(1);
  assert(x0 == 11);
  assert(x1 == 9);

  cout << "Test 1 ok!" << endl;
}

int main() {
  cout << "starting peer service tests..." << endl;
  test1();
}
