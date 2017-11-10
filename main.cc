#include "udp.h"

using namespace std;

int main()
{
    UDPServer server("3000");

    UDPClient client("localhost", "3000");
    client.send("OLAAA");

    sleep(1000);

  return 0;
}
