#include "../include/peer_service.h"

using boost::asio::ip::udp;
using namespace std;

// UDPClient
UDPClient::UDPClient(const string& host, const int& port)
  : socket_(io_service_, udp::endpoint(udp::v4(), 0))
{
  udp::resolver resolver(io_service_);
  udp::resolver::query query(udp::v4(), host, to_string(port));
  udp::resolver::iterator iter = resolver.resolve(query);
  endpoint_ = *iter;
}

UDPClient::~UDPClient()
{
  this->close();
}

void UDPClient::send(const stringstream& ss)
{
  socket_.send_to(
      boost::asio::buffer(ss.str().data(), ss.str().size()),
      endpoint_
  );
}

void UDPClient::close()
{
  socket_.close();
}

// UDPServer
UDPServer::UDPServer(const int& port)
  : socket_(io_service_, udp::endpoint(udp::v4(), port))
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
      endpoint_,
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
  cout << message << endl;
  socket_.send_to(boost::asio::buffer(message), endpoint_);

  start_receive();
}

// PeerService
PeerService::PeerService(const int& port)
  : server_(port) { }

PeerService::~PeerService()
{
  for(auto it = peers_.begin(); it != peers_.end(); ++it)
    it->second->close();
}

void PeerService::join(const int& id, const string& host, const int& port)
{
  UDPClient* client = new UDPClient(host, port);
  peers_[id] = client;
}
