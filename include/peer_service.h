#ifndef __PEER_SERVICE_H_INCLUDED__
#define __PEER_SERVICE_H_INCLUDED__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "util.h"

using std::cout;
using std::cerr;
using std::endl;
using std::unordered_map;

/*
 For info on UDP see:
   https://www.abc.se/~m6695/udp.html
*/

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

struct PeerSpec {
  string ip;
  int port;
};

struct PeerSock {
  bool valid;
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
    if(r) {
      cerr << "Error creating reader thread (start)" << endl;
      return;
    }
  }
  
  vector<int> members() {
    // all peers that ever joined
    vector<int> res;
    for(auto it = specs_.begin(); it != specs_.end(); ++it) {
      res.push_back(it->first);
    }
    return res;
  }

  void join(string spec) {
    // assumes a string in the form id:ip:port
    vector<string> parts = str_split(spec, ':');
    int peer_id = stoi(parts.at(0));
    string peer_ip = parts.at(1);
    int peer_port = stoi(parts.at(2));
    join(peer_id, peer_ip, peer_port);
  }

  void join(int id, const string& ip, const int& port) {
    // store peer spec
    struct PeerSpec spec;
    spec.ip = ip;
    spec.port = port;
    specs_[id] = spec;
    // TODO write to disk

    // connect to peer
    struct PeerSock sock = connect(ip, port);
    cout << "Attempt to join " << id << " on " << ip << ":" << port;
    if(sock.valid) {
      socks_[id] = sock;
      cout << " succeeded" << endl;
    } else cout << " failed" << endl;
  }

  void send(int id, const char* buf, int len) {
    assert(len <= MAX_DATAGRAM_SIZE);
    auto spec = specs_.find(id);
    if(spec == specs_.end()) {
      cerr << "Peer " << id << " was never connected!" << endl;
      return;
    }

    auto sock = socks_.find(id);
    if(sock == socks_.end()) {
      // after a reboot
      sock->second = connect(spec->second.ip, spec->second.port);
    }

    // assume it's connected and try to send
    // if error, connect in again
    int ras = sizeof(sock->second.addr);
    int r = sendto(
      sock->second.socket,
      buf,
      len,
      0,
      (struct sockaddr *) &sock->second.addr,
      ras
    );
    if(r == -1) {
      cerr << "Error sending message to " << id << endl;
      sock->second = connect(spec->second.ip, spec->second.port);
    }
  }

private:
  struct PeerSock connect(const string& ip, const int& port) {
    struct PeerSock res;
    res.valid = false;

    // open socket
    int sock;
    if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      cerr << "Error creating socket (join)" << endl;
      return res;
    }

    // bind ip addres
    struct sockaddr_in remote_addr;
    memset((char *) &remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);
    if(inet_aton(ip.c_str(), &remote_addr.sin_addr) == 0) {
      cerr << "Error connecting to address (join)" << endl;
      return res;
    }

    res.valid = true;
    res.addr = remote_addr;
    res.socket = sock;

    return res;
  }

};

template<typename T>
void *reader(void *in) {
  struct ReaderArgs<T>* args = (ReaderArgs<T>*) in;
  int r;
  // open socket
  int sock;
  if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    cerr << "Error creating socket (reader)" << endl;
    exit(1);
  }

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
  if(r == -1) {
    cerr << "Error binding address (reader)" << endl;
    exit(1);
  } else cout << "Listening on " << inet_ntoa(local_addr.sin_addr) << ":" << args->port << endl;

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
    if(len == -1) cerr << "Error reading from socket (reader)" << endl;
    else {
      //char *ip_from = inet_ntoa(remote_addr.sin_addr);
      //int port_from = ntohs(remote_addr.sin_port);
      //cout << "Received " << buf << " from "
      //     << ip_from << ":" << port_from << endl;
      args->w->apply(buf, len);
    }
  }
}

#endif
