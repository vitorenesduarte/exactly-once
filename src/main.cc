#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm> // random_shuffle min
#include "../include/monoids.h"
#include "../include/handoff.h"
#include "../include/peer_service.h"

using std::cout;
using std::endl;
using std::vector;

typedef Handoff<int> MyHandoff;

class MyWrapper : public Wrapper<MyHandoff> {
public:
  MyWrapper(MyHandoff* t) : Wrapper(t) { }
  void apply(char* buf, int len) {
    MyHandoff h;
    h.unpack(buf, len);
    t_->merge(h);
    cout << t_->id << "> received handoff from " << h.id <<  endl;
    cout << t_->id << "> current val: " << t_->val << endl;
  }
};

struct SyncArgs {
  int fanout;
  int interval;
  MyHandoff* h;
  PeerService<MyHandoff>* ps;
};

void *sync(void *in) {
  SyncArgs* args = (SyncArgs*) in;

  while(1) {
    // sleep
    std::this_thread::sleep_for(
        std::chrono::milliseconds(args->interval)
    );

    // shuffle members
    vector<int> members = args->ps->members();
    random_shuffle(members.begin(), members.end());

    // interate shuffled list
    //  - size of members if fanout is 0
    //  - min between fanout and size of members (otherwise)
    int until;
    if(args->fanout == 0) until = members.size();
    else until = std::min<int>(args->fanout, members.size());

    for(int i = 0; i < until; ++i) {
      int member = members.at(i);
      // cout << args->h->id << "> sending handoff to " << member << endl;

      // serialize in `ss` the information about this `member`
      stringstream ss;
      args->h->pack(ss, member);

      // send
      args->ps->send(
        member,
        ss.str().c_str(),
        ss.str().size()
      );
    }
  }
}

int main(int argc, char** argv) {

  // default id
  int id = 0;
  // default port
  int port = 3000;
  // default connections
  vector<string> connections;
  // default fanout
  int fanout = 1;
  // default interval (milliseconds)
  int interval = 1000;

  int opt;
  while((opt = getopt(argc, argv, "i:p:c:f:t:")) != EOF) {
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
      case 'f':
        fanout = atoi(optarg);
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
  cout << "fanout: " << fanout << endl;
  cout << "interval: " << interval << endl;

  MyHandoff h(id, zero());
  Wrapper<MyHandoff>* w = new MyWrapper(&h);
  PeerService<MyHandoff>* ps = new PeerService<MyHandoff>;
  ps->start(port, w);
  sleep(2);

  for(auto it = connections.begin(); it != connections.end(); ++it) {
    ps->join(*it);
  }

  // start sync
  pthread_t tr;
  SyncArgs* args = new SyncArgs;
  args->fanout = fanout;
  args->interval = interval;
  args->h = &h;
  args->ps = ps;

  int r = pthread_create(&tr, NULL, sync, (void *) args);
  if(r) {
    cerr << "Error creating sync thread (main)" << endl;
    exit(1);
  }

  for(int i = 1; i <= 10; i++) {
     sleep(rand() % 5);
     h.plus(id * (i * 10));
  }

  sleep(300);

  return 0;
}
