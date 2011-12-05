#ifndef __DARC_UDP_LINK_H_INCLUDED__
#define __DARC_UDP_LINK_H_INCLUDED__

#include <boost/asio.hpp>
#include <darc/serialized_message.h>
#include <darc/node_link.h>
#include <darc/shared_buffer.h>
#include <darc/packet/header.h>
#include <darc/packet/message.h>

namespace darc
{
namespace udp
{

class Link : public darc::NodeLink
{
public:
  typedef boost::shared_ptr<udp::Link> Ptr;

private:
  boost::asio::io_service * io_service_;
  
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint remote_endpoint_;
  
  unsigned int local_port_;

  std::map<uint32_t, boost::asio::ip::udp::endpoint> endpoints_;

public:
  Link(boost::asio::io_service * io_service, unsigned int local_port):
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
                                boost::bind(&Link::handleReceive, this,
				recv_buffer,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
  }

  void addRemoteNode( uint32_t remote_node_id, const std::string& host, const std::string& port)
  {
    // todo: do it async
    boost::asio::ip::udp::resolver resolver(*io_service_);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, port);
    endpoints_[remote_node_id] = *resolver.resolve(query);
  }

  // impl of virtual
  void dispatchToRemoteNode( uint32_t remote_node_id, const std::string& topic, SerializedMessage::ConstPtr msg)
  {
    // todo: to do an async send_to, msg must be kept alive until the send is finished. How to do this?
    //       Impl a object fulfilling the boost buffer interface which holds the smart pointer internally....
    //       Dude.... This is all messy right now....

    // Send Header
    packet::Header header(node_id_, packet::Header::MSG_PACKET);
    packet::Message msg_header(topic);

    boost::array<uint8_t, 512> tmp_buf;

    size_t pos = header.write( tmp_buf.data(), tmp_buf.size() );
    pos += msg_header.write( tmp_buf.data() + pos, tmp_buf.size() - pos );

    boost::array<boost::asio::const_buffer, 2> tmp_bufs = {{
	boost::asio::buffer(tmp_buf.data(), pos),
	boost::asio::buffer(msg->getBuffer().data(), msg->getBuffer().size())
      }};
    
    // Send Msg packet

    socket_.send_to(tmp_bufs, endpoints_[remote_node_id]);
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

      packet::Header header;
      header.read( recv_buffer.data(), size );
      
      // switch on packet type
      if (header.payload_type == packet::Header::MSG_PACKET)
      {
	recv_buffer.addOffset( packet::Header::size() );
	packet::Message msg_packet;
	size_t msg_header_size = msg_packet.read( recv_buffer.data(), size - packet::Header::size() );
	recv_buffer.addOffset( msg_header_size );
	SerializedMessage::Ptr msg_s( new SerializedMessage( recv_buffer , size - packet::Header::size() - msg_header_size ) );
	receive_callback_( header.sender_node_id, msg_packet.topic, msg_s );
      }
      else
      {
	std::cout << "Unknown packet type received??" << std::endl;
      }
    }
    startReceive();
  }

};

} // namespace udp
} // namespace darc

#endif
