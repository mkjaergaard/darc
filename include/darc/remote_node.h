#ifndef __DARC_REMOTE_NODE_H_INCLUDED__
#define __DARC_REMOTE_NODE_H_INCLUDED__

#include <boost/asio.hpp>

namespace darc
{
  
  // todo: make this an abstract class wrapping different ways to connect remotely
class LocalDispatcher
{
  virtual void LocalDispatch( const std::string& topic, MsgWrappedAbstract::Ptr msg ) = 0;
};
  
class RemoteDispatcher
{
  virtual void RemoteDispatch( const std::string& topic, MsgWrappedAbstract::Ptr msg ) = 0;
};

class UDPSource
{
  boost::io_service * io_service_;
  LocalDispatcher * local_dispatcher_;
  
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 4098> recv_buffer_;
  udp::endpoint remote_endpoint_;  

  UDPSource(boost::io_service * io_service, LocalDispatcher * local_dispatcher, unsigned int local_port):
    io_service_(io_service),
    socket_(io_service, udp::endpoint(udp::v4(), port)),
  {
  }
  
  StartReceive()
  {
    socket_.async_receive_from( boost::asio::buffer(recv_buffer_), remote_endpoint_,
                                boost::bind(&UDPSource::HandleReceive, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
  }

 public:
  void handle_receive(const boost::system::error_code& error,
                      std::size_t)
  {

  }

  void DispatchToRemoteNode( const std::string& topic, MsgWrappedAbstract::Ptr msg )
  {
  }
  
};

}

#endif
