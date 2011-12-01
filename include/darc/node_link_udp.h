#ifndef __DARC_NODE_LINK_UDP_H_INCLUDED__
#define __DARC_NODE_LINK_UDP_H_INCLUDED__

#include <boost/asio.hpp>

#include <darc/serialized_message.h>
#include <darc/node_link.h>
#include <darc/shared_buffer.h>

namespace darc
{

class NodeLinkUDP : public NodeLink
{
public:
  typedef boost::shared_ptr<NodeLinkUDP> Ptr;

private:
  boost::asio::io_service * io_service_;
  
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint remote_endpoint_;
  
  uint8_t recv_buffer_[4098];

  unsigned int local_port_;

  std::map<int, boost::asio::ip::udp::endpoint> endpoints_;

public:
  NodeLinkUDP(boost::asio::io_service * io_service, unsigned int local_port):
    io_service_(io_service),
    socket_(*io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), local_port)),
    local_port_(local_port)
  {
    startReceive();
  }
  
  void startReceive()
  {
    SharedBuffer recv_buffer = SharedBuffer::create(4098);
    socket_.async_receive_from( boost::asio::buffer(recv_buffer.data(), recv_buffer.size()), remote_endpoint_,
                                boost::bind(&NodeLinkUDP::handleReceive, this,
				recv_buffer,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
  }

  void addRemoteNode( int id, const std::string& host, const std::string& port)
  {
    // todo: do it async
    boost::asio::ip::udp::resolver resolver(*io_service_);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, port);
    endpoints_[id] = *resolver.resolve(query);
  }

  // impl of virtual
  void dispatchToRemoteNode( int id, SerializedMessage::ConstPtr msg)
  {
    // todo: to do an async send_to, msg must be kept alive until the send is finished. How to do this?
    //       Impl a object fulfilling the boost buffer interface which holds the smart pointer internally....
    socket_.send_to(boost::asio::buffer(msg->getBuffer().data(), msg->getBuffer().size()), endpoints_[id]);
  }

 public:
  void handleReceive(SharedBuffer recv_buffer, const boost::system::error_code& error, std::size_t size)
  {
    if ( error )
    {
      std::cerr << "read error: " << boost::system::system_error(error).what() << std::endl;
    }
    else
    {
      std::cout << "Received: " << size << " on port " << local_port_ << std::endl;
      SerializedMessage::Ptr msg_s( new SerializedMessage(recv_buffer, size) );
      receive_callback_(1, msg_s);
    }
    startReceive();
  }

};

}

#endif
