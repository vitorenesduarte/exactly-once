#include <iostream>
#include <string>
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
    UDPClient(const std::string& host, const std::string& port);
    ~UDPClient();
    void send(const std::string& msg);
};

class UDPServer
{
private:
  boost::asio::io_service io_service_;
  boost::thread thread_;
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, MAX_DATAGRAM_SIZE> recv_buffer_;

public:
  UDPServer(const int port);
  void spawn();

private:
  void start_receive();
  void handle_receive(const boost::system::error_code& error,
      size_t bytes_transferred);
};
