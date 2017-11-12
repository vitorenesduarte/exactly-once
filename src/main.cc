#include <unistd.h>
#include "../include/peer_service.h"

using std::cout;
using std::endl;

int main() {
  peer_service(3000);
  sleep(1000);
  return 0;
}
