#include <unistd.h>
#include "../include/peer_service.h"

using std::cout;
using std::endl;

class HandoffWrapper : public Wrapper<int> {
public:
  HandoffWrapper(int* x) : Wrapper(x) { }
  ~HandoffWrapper() { }
  void apply(char* buf, int len) {
    cout << *t_ << endl;
    cout << buf << endl;
    cout << len << endl;
  }
};

int main() {
  int x = 3;
  Wrapper<int>* wrapper = new HandoffWrapper(&x);
  peer_service(3000, wrapper);
  sleep(1000);
  return 0;
}
