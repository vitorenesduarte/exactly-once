#ifndef __PEER_SERVICE_H_INCLUDED__
#define __PEER_SERVICE_H_INCLUDED__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <unordered_map>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::unordered_map;

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

template<typename T>
void *reader(void *in);

void warn(string msg);

struct PeerSpec {
  string ip;
  int port;
};

struct PeerSock {
  struct sockaddr_in addr;
  int socket;
};

template<typename T>
class PeerService
{
private:
  unordered_map<int, PeerSpec> specs_;
  unordered_map<int, PeerSock> socks_;

public:
  PeerService() { }

  void start(const int& port, Wrapper<T>* w) {
    pthread_t tr;
    ReaderArgs<T>* args = new ReaderArgs<T>;
    args->port = port;
    args->w = w;

    int r = pthread_create(&tr, NULL, reader<T>, (void *) args);
    if(r) warn("Error creating reader thread (start)");
  }

  void join(int id, const string& ip, const int& port) {
    int r;

    // store peer spec for possible reconnect
    struct PeerSpec spec;
    spec.ip = ip;
    spec.port = port;
    specs_[id] = spec;

    // connect to peer
    struct PeerSock sock = connect(ip, port);
    socks_[id] = sock;
  }

  void send(int id, char* buf, int len) {

  }

private:
  struct PeerSock connect(const string& ip, const int& port) {
    // open socket
    int sock;
    if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
      warn("Error creating socket (join)");

    // bind ip address
    struct sockaddr_in remote_addr;
    memset((char *) &remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);
    if(inet_aton(ip.c_str(), &remote_addr.sin_addr) == 0)
      warn("Error connecting to address (join)");

    struct PeerSock ps;
    ps.addr = remote_addr;
    ps.socket = sock;

    return ps;
  }

};

template<typename T>
void *reader(void *in) {
  struct ReaderArgs<T>* args = (ReaderArgs<T>*) in;
  int r;
  // open socket
  int sock;
  if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    warn("Error creating socket (reader)");

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
  if(r == -1) warn("Error binding address (reader)");

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
    if(len == -1) warn("Error reading from socket (reader)");

    //char *ip_from = inet_ntoa(remote_addr.sin_addr);
    //int port_from = ntohs(remote_addr.sin_port);
    //cout << "Received " << buf << " from "
    //     << ip_from << ":" << port_from << endl;
    args->w->apply(buf, len);
  }
}

void warn(string msg) {
  cerr << msg << endl;
}

#endif
