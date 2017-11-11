#ifndef __PEER_SERVICE_H_INCLUDED__
#define __PEER_SERVICE_H_INCLUDED__

#include <iostream>
#include <string>
#include <unordered_map>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;
using namespace std;

const unsigned int MAX_DATAGRAM_SIZE = 65536;

class UDPClient
{
private:
  boost::asio::io_service io_service_;
  udp::socket socket_;
  udp::endpoint endpoint_;

public:
  UDPClient(const string& host, const int& port);
  ~UDPClient();
  void send(const stringstream& msg);
  void close();
};

class UDPServer
{
private:
  boost::asio::io_service io_service_;
  boost::thread thread_;
  udp::socket socket_;
  udp::endpoint endpoint_;
  boost::array<char, MAX_DATAGRAM_SIZE> recv_buffer_;

public:
  UDPServer(const int& port);

private:
  void start_receive();
  void handle_receive(const boost::system::error_code& error,
      size_t bytes_transferred);
};

class PeerService
{
private:
  UDPServer server_;
  unordered_map<int, UDPClient*> peers_;

public:
  PeerService(const int& port);
  ~PeerService();
  void join(const int& id, const string& host, const int& port);
};

#endif
