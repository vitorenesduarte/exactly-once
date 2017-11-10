#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;
using namespace std;

class UDPClient
{
private:
    boost::asio::io_service io_service_;
    udp::socket socket_;
    udp::endpoint endpoint_;

public:
    UDPClient(const std::string& host, const std::string& port)
      : socket_(io_service_, udp::endpoint(udp::v4(), 0))
    {
        udp::resolver resolver(io_service_);
        udp::resolver::query query(udp::v4(), host, port);
        udp::resolver::iterator iter = resolver.resolve(query);
        endpoint_ = *iter;
    }

    ~UDPClient()
    {
        socket_.close();
    }

    void send(const std::string& msg) {
        socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
    }
};

class UDPServer
{
private:
  boost::asio::io_service io_service_;
  boost::thread thread_;
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  // FIXME 1024
  boost::array<char, 1024> recv_buffer_;

public:
  UDPServer(const int port)
    : socket_(io_service_, udp::endpoint(udp::v4(), port)) { }

  void spawn()
  {
    start_receive();
    thread_ = boost::thread(
      boost::bind(&boost::asio::io_service::run, &io_service_)
    );
  }

private:
  void start_receive()
  {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_),
        remote_endpoint_,
        boost::bind(
          &UDPServer::handle_receive,
          this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred
        )
    );
  }

  void handle_receive(const boost::system::error_code& error,
      size_t /*bytes_transferred*/)
  {
    if (error && error != boost::asio::error::message_size)
    {
      throw boost::system::system_error(error);
    }

    string message(recv_buffer_.begin(), recv_buffer_.end());
    socket_.send_to(boost::asio::buffer(message), remote_endpoint_);

    start_receive();
  }
};

int main()
{
    UDPServer server(3000);
    server.spawn();

    UDPClient client("localhost", "3000");
    client.send("OLAAA");

    sleep(1000);

  return 0;
}
