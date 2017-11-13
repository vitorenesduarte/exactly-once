#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include "../include/monoids.h"
#include "../include/handoff.h"
#include "../include/peer_service.h"

using std::cout;
using std::endl;
using std::vector;

typedef Handoff<int> MyHandoff;

class MyWrapper : public Wrapper<MyHandoff> {
public:
  MyWrapper(MyHandoff* x) : Wrapper(x) { }
  void apply(char* buf, int len) {
    MyHandoff h;
    h.unpack(buf, len);
    t_->merge(h);
    cout << "received handoff from " << h.id <<  endl;
    cout << "current val: " << t_->val << endl;
  }
};

// https://stackoverflow.com/a/236803/4262469
template<typename Out>
void str_split(const string &s, char delim, Out result) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}
vector<string> str_split(const string &s, char delim) {
	vector<string> elems;
	str_split(s, delim, back_inserter(elems));
	return elems;
}

int main(int argc, char** argv) {

  // default id
  int id = 0;
  // default port
  int port = 3000;
  // default connections
  vector<string> connections;

  int opt;
  while((opt = getopt(argc, argv, "i:p:c:")) != EOF) {
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

  MyHandoff h(id, zero());
  Wrapper<MyHandoff>* w = new MyWrapper(&h);
  PeerService<MyHandoff> ps;
  ps.start(port, w);
  sleep(2);

  for(auto it = connections.begin(); it != connections.end(); ++it) {
    vector<string> parts = str_split(*it, ':');
    int peer_id = stoi(parts.at(0));
    string peer_ip = parts.at(1);
    int peer_port = stoi(parts.at(2));
    ps.join(peer_id, peer_ip, peer_port);
  }

  h.plus(id);

  for(int i = 0; i < 10; i++) {
    vector<int> members = ps.members();
    int j = rand() % members.size();
    int member = members.at(j);

    stringstream ss;
    h.pack(ss);

    ps.send(member, ss.str().c_str(), ss.str().size());
    sleep(rand() % 5);
  }
  sleep(10);
  return 0;
}
