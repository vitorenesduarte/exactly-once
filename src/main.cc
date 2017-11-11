#include "../include/peer_service.h"

using namespace std;

int main()
{
    PeerService(3000);

    // UDPClient client("localhost", 3000);
    // stringstream ss;
    // ss << "HELLO" << endl;
    // client.send(ss);
    sleep(1000);

  return 0;
}
