#ifndef __DISTRIBUTION_H_INCLUDED__
#define __DISTRIBUTION_H_INCLUDED__

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
  UDPClient(const string& host, const string& port);
  ~UDPClient();
  void send(const string& msg);
};

//class PeerService
//{
//private:
//  unordered_map<int, UDPClient> peers;

//public:
//  PeerService();
//  void join(int id, const string& host, const string& port);
//};


class UDPServer
{
private:
  boost::asio::io_service io_service_;
  boost::thread thread_;
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, MAX_DATAGRAM_SIZE> recv_buffer_;

public:
  UDPServer(const string& port);

private:
  void start_receive();
  void handle_receive(const boost::system::error_code& error,
      size_t bytes_transferred);
};

#endif
