#ifndef __PEER_SERVICE_H_INCLUDED__
#define __PEER_SERVICE_H_INCLUDED__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;

const unsigned int MAX_DATAGRAM_SIZE = 65536;

template<typename T>
class Wrapper {
public:
  T *t_;
  Wrapper(T* t) : t_(t) { }
  virtual void apply(char* buf, int len) = 0;
};

template<typename T>
struct ReaderArgs {
  int port;
  Wrapper<T>* w;
};

void diep(string msg) {
  cerr << msg << endl;
  exit(1);
}

template<typename T>
void *reader(void *in);

template<typename T>
void peer_service(const int& port, Wrapper<T>* w) {
  pthread_t tr;
  ReaderArgs<T>* args = new ReaderArgs<T>;
  args->port = port;
  args->w = w;

  int r = pthread_create(&tr, NULL, reader<T>, (void *) args);
  if(r) diep("Error creating reader thread");
}

template<typename T>
void *reader(void *in) {
  struct ReaderArgs<T>* args = (ReaderArgs<T>*) in;
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
    int ras = sizeof(remote_addr);
    int len = recvfrom(
      sock,
      buf,
      MAX_DATAGRAM_SIZE,
      0,
      (struct sockaddr *) &remote_addr,
      (socklen_t *) &ras
    );
    if(len == -1) diep("Error reading from socket");

    //char *ip_from = inet_ntoa(remote_addr.sin_addr);
    //int port_from = ntohs(remote_addr.sin_port);
    //cout << "Received " << buf << " from "
    //     << ip_from << ":" << port_from << endl;
    args->w->apply(buf, len);
  }
}

#endif
