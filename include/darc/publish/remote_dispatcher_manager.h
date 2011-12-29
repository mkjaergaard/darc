#ifndef __DARC_PUBLISH_REMOTE_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_PUBLISH_REMOTE_DISPATCH_HANDLER_H_INCLUDED__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <darc/serialized_message.h>
#include <darc/node_link.h>
#include <darc/node_link_manager.h>
#include <darc/packet/header.h>

namespace darc
{
namespace publish
{

class RemoteDispatcherManager
{
private:
  boost::asio::io_service * io_service_;
  NodeLinkManager link_manager_;

  // Function to dispatch locally
  typedef boost::function<void (const std::string& topic, SerializedMessage::ConstPtr)> LocalDispatchFunctionType;
  LocalDispatchFunctionType local_dispatch_function_;

  // Function to send to remote node
  typedef boost::function<void (packet::Header::PayloadType, SharedBuffer, std::size_t )> SendToNodeFunctionType;
  SendToNodeFunctionType send_to_node_function_;

public:
  RemoteDispatcherManager( boost::asio::io_service * io_service ) :
    io_service_( io_service ),
    link_manager_( io_service )
  {
  }

  void packetReceiveHandler( SharedBuffer buffer, std::size_t data_len )
  {
    // Parse Msg Packet
    packet::Message msg_packet;
    size_t msg_header_size = msg_packet.read( buffer.data(), data_len );
    buffer.addOffset( msg_header_size );

    // Extract Serialized Message
    SerializedMessage::Ptr msg_s( new SerializedMessage( buffer, data_len - msg_header_size ) );

    // Dispatch to local subscribers
    local_dispatch_function_( msg_packet.topic, msg_s );
  }

  void setLocalDispatchFunction( LocalDispatchFunctionType local_dispatch_function )
  {
    local_dispatch_function_ = local_dispatch_function;
  }

  void setSendToNodeFunction( SendToNodeFunctionType send_to_node_function )
  {
    send_to_node_function_ = send_to_node_function;
  }

  // Triggered by asio post
  template<typename T>
  void serializeAndDispatch( const std::string topic, const boost::shared_ptr<const T> msg )
  {
    // Allocate buffer. todo: derive required size from msg
    std::size_t data_len = 1024;
    SharedBuffer buffer = SharedBuffer::create(data_len);

    // Message Header
    packet::Message msg_header(topic);
    std::size_t pos = msg_header.write( buffer.data(), buffer.size() );

    // todo: Put common serialization stuff somewhere to reuse
    // Write Type Info
    pos += packet::Parser::writeString( ros::message_traits::DataType<T>::value(), buffer.data() + pos, buffer.size() - pos );
    // MD5
    pos += packet::Parser::writeUint64( ros::message_traits::MD5Sum<T>::static_value1, buffer.data() + pos, buffer.size() - pos );
    pos += packet::Parser::writeUint64( ros::message_traits::MD5Sum<T>::static_value2, buffer.data() + pos, buffer.size() - pos );

    // Serialize actual message
    ros::serialization::OStream ostream( buffer.data() + pos, buffer.size() - pos );
    ros::serialization::serialize( ostream, *(msg.get()) );

    assert( send_to_node_function_ );
    send_to_node_function_( packet::Header::MSG_PACKET, buffer, data_len );
  }

  // Called by LocalDispatcher
  template<typename T>
  void postRemoteDispatch( const std::string& topic, const boost::shared_ptr<const T> msg )
  {
    // if( remote subscribers )
    io_service_->post( boost::bind(&RemoteDispatcherManager::serializeAndDispatch<T>, this, topic, msg) );
  }

};

}
}

#endif
