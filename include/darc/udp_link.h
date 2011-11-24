#ifndef __DARC_REMOTE_NODE_H_INCLUDED__
#define __DARC_REMOTE_NODE_H_INCLUDED__

#include <boost/asio.hpp>

#include <darc/serialized_message.h>
#include <darc/remote_node_link.h>

namespace darc
{

  // todo: find a better name for the callback
class LinkCallback
{
public:
  virtual void MsgReceived( const std::string &topic, boost::shared_ptr<SerializedMessage> msg_s ) = 0;
};
  
class UDPLink : public RemoteNodeLink
{
  boost::asio::io_service * io_service_;
  LinkCallback * link_callback_;
  
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint remote_endpoint_;
  boost::array<char, 4098> recv_buffer_;
  unsigned int local_port_;

  std::map<int, boost::asio::ip::udp::endpoint> endpoints_;

 public:
 UDPLink(boost::asio::io_service * io_service,/* LinkCallback * link_callback,*/ unsigned int local_port):
    io_service_(io_service),
      /*    link_callback_(link_callback),*/
      socket_(*io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), local_port)),
      local_port_(local_port)
  {
    startReceive();
  }
  
  void startReceive()
  {
    socket_.async_receive_from( boost::asio::buffer(recv_buffer_), remote_endpoint_,
                                boost::bind(&UDPLink::handleReceive, this,
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
    socket_.send_to(boost::asio::buffer(msg.get()->data_, msg.get()->data_size_), endpoints_[id]);
  }

 public:
  void handleReceive(const boost::system::error_code& error, std::size_t size)
  {
    std::cout << "Received: " << size << " on port " << local_port_ << std::endl;
    startReceive();
  }

};

}

#endif
