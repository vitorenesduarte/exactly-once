#include <unistd.h>
#include "../include/peer_service.h"

using std::cout;
using std::endl;

class HandoffWrapper : public Wrapper<int> {
public:
  HandoffWrapper(int* x) : Wrapper(x) { }
  void apply(char* buf, int len) {
    cout << "T: " << *t_ << endl;
    cout << "BUF: " << buf << endl;
    cout << "LEN: " << len << endl;
  }
};

int main() {
  int x0 = 17;
  Wrapper<int>* w0 = new HandoffWrapper(&x0);
  PeerService<int> ps0;
  ps0.start(3000, w0);
  sleep(1);

  int x1 = 23;
  Wrapper<int>* w1 = new HandoffWrapper(&x1);
  PeerService<int> ps1;
  ps1.start(3001, w1);
  sleep(1);

  // ps0 joins ps1
  ps0.join(1, "127.0.0.1", 3001);

  // ps0 sends message to ps1
  string msg = "HELLO";
  ps0.send(1, msg.c_str(), msg.size());
  sleep(30);

  return 0;
}
