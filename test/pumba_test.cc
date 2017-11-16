#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <chrono>
#include "../include/peer_service.h"

using std::cout;
using std::endl;
using std::vector;

typedef vector<int> Wrapped;

class MyWrapper : public Wrapper<Wrapped> {
public:
  MyWrapper(Wrapped* t) : Wrapper(t) { }
  void apply(char* buf, int len) {
    stringstream ss(buf);
    int recv;
    ss >> recv;
    t_->push_back(recv);
    cout << "received " << recv << endl;
  }
};

struct SyncArgs {
  int until;
  int interval;
  Wrapped* h;
  PeerService<Wrapped>* ps;
};

void *sync(void *in) {
  SyncArgs* args = (SyncArgs*) in;

  for(int u = 0; u < args->until; ++u) {
    // sleep
    std::this_thread::sleep_for(
        std::chrono::milliseconds(args->interval)
    );

    vector<int> members = args->ps->members();

    for(int i = 0; i < members.size(); ++i) {
      int member = members.at(i);

      string ss = std::to_string(u);

      // send
      args->ps->send(
        member,
        ss.c_str(),
        ss.size()
      );
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char** argv) {

  // default id
  int id = 0;
  // default port
  int port = 3000;
  // default connections
  vector<string> connections;
  // default until
  int until = 20;
  // default interval (milliseconds)
  int interval = 1000;

  int opt;
  while((opt = getopt(argc, argv, "i:p:c:u:t:")) != EOF) {
    switch(opt) {
      case 'i':
        cout << optarg << endl;
        id = atoi(optarg);
        break;
      case 'p':
        port = atoi(optarg);
        break;
      case 'c':
        connections.push_back(optarg);
        break;
      case 'u':
        until = atoi(optarg);
        break;
      case 't':
        interval = atoi(optarg);
        break;
      default:
        cerr << "Unknown option: " << opt << endl;
        exit(1);
    }
  }

  cout << "id: " << id << endl;
  cout << "port: " << port << endl;
  cout << "connections: " << endl;
  for(auto it = connections.begin(); it != connections.end(); ++it) {
    cout << " - " << *it << endl;
  }
  cout << "until: " << until << endl;
  cout << "interval: " << interval << endl;

  Wrapped h;
  Wrapper<Wrapped>* w = new MyWrapper(&h);
  PeerService<Wrapped>* ps = new PeerService<Wrapped>;
  ps->start(port, w);
  sleep(2);

  for(auto it = connections.begin(); it != connections.end(); ++it) {
    ps->join(*it);
  }

  // start sync
  pthread_t tr;
  SyncArgs* args = new SyncArgs;
  args->until = until;
  args->interval = interval;
  args->h = &h;
  args->ps = ps;

  int r = pthread_create(&tr, NULL, sync, (void *) args);
  if(r) {
    cerr << "Error creating sync thread (main)" << endl;
    exit(1);
  }

  pthread_join(tr, NULL);
  for(auto it = h.begin(); it != h.end(); ++it)
    cout << *it << " ";
  cout << endl;

  sleep(10);
  for(auto it = h.begin(); it != h.end(); ++it)
    cout << *it << " ";
  cout << endl;

  return 0;
}
