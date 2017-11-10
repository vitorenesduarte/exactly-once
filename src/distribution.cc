#include "../include/distribution.h"

using boost::asio::ip::udp;
using namespace std;

// UDPClient
UDPClient::UDPClient(const string& host, const string& port)
  : socket_(io_service_, udp::endpoint(udp::v4(), 0))
{
  udp::resolver resolver(io_service_);
  udp::resolver::query query(udp::v4(), host, port);
  udp::resolver::iterator iter = resolver.resolve(query);
  endpoint_ = *iter;
}

UDPClient::~UDPClient()
{
  socket_.close();
}

void UDPClient::send(const string& msg) {
  socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
}

// UDPServer
UDPServer::UDPServer(const string& port)
  : socket_(io_service_, udp::endpoint(udp::v4(), stoi(port)))
{
  start_receive();
  thread_ = boost::thread(
    boost::bind(&boost::asio::io_service::run, &io_service_)
  );
}

void UDPServer::start_receive()
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

void UDPServer::handle_receive(const boost::system::error_code& error,
    size_t bytes_transferred)
{
  if (error && error != boost::asio::error::message_size)
  {
    throw boost::system::system_error(error);
  }

  string message(recv_buffer_.begin(), bytes_transferred);
  socket_.send_to(boost::asio::buffer(message), remote_endpoint_);

  start_receive();
}
