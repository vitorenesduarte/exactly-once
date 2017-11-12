#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../include/peer_service.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;

const unsigned int MAX_DATAGRAM_SIZE = 65536;

struct ReaderArgs {
  int port;
};

void diep(string msg) {
  cerr << msg << endl;
  exit(1);
}
void *reader(void *in);

void peer_service(const int& port) {
  pthread_t tr;
  ReaderArgs* args = new ReaderArgs;
  args->port = port;

  cout << "ATRR " << args->port << endl;
  int r = pthread_create(&tr, NULL, reader, (void *) args);
  if(r) diep("Error creating reader thread");
}

void *reader(void *in) {
  struct ReaderArgs* args = (ReaderArgs*) in;
  cout << "READER STARTED ON " << args->port << endl;
  int r;
  // open socket
  int sock;
  if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    diep("Error creating socket");

  // bind ip address
  struct sockaddr_in local_addr;
  memset((char *) &local_addr, 0, sizeof(local_addr));
  local_addr.sin_family = AF_INET;
  local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  local_addr.sin_port = htons(args->port);
  r = bind(
    sock,
    (struct sockaddr *) &local_addr,
    sizeof(local_addr)
  );
  if(r == -1) diep("Error binding address");

  // listen new messages forever
  char buf[MAX_DATAGRAM_SIZE];
  struct sockaddr_in remote_addr;

  while(1) {
    int len = sizeof(remote_addr);
    cout << "Waiting" << endl;
    r = recvfrom(
      sock,
      buf,
      MAX_DATAGRAM_SIZE,
      0,
      (struct sockaddr *) &remote_addr,
      (socklen_t *) &len
    );
    if(r == -1) diep("Error reading from socket");

    //char *ip_from = inet_ntoa(remote_addr.sin_addr);
    //int port_from = ntohs(remote_addr.sin_port);
    //cout << "Received " << buf << " from "
    //     << ip_from << ":" << port_from << endl;
    cout << buf << endl;
  }
}
