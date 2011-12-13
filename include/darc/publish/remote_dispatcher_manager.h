#ifndef __DARC_PUBLISH_REMOTE_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_PUBLISH_REMOTE_DISPATCH_HANDLER_H_INCLUDED__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <darc/serialized_message.h>
#include <darc/node_link.h>
#include <darc/node_link_manager.h>

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

  // Remote Connections
  typedef std::map<uint32_t, NodeLink::Ptr > ConnectionListType;
  ConnectionListType connection_list_;

  typedef std::vector<NodeLink::Ptr> AcceptorListType;
  AcceptorListType acceptor_list_;

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
    std::cout << "Received a message for topic " << msg_packet.topic << std::endl;
    local_dispatch_function_( msg_packet.topic, msg_s );
  }

  void setLocalDispatchFunction( LocalDispatchFunctionType local_dispatch_function )
  {
    local_dispatch_function_ = local_dispatch_function;
  }

  // Triggered by asio post
  template<typename T>
  void serializeAndDispatch( const std::string topic, const boost::shared_ptr<const T> msg )
  {
    SerializedMessage::Ptr msg_s( new SerializedMessage(msg) );
    // todo find the right nodes to dispatch to
    for( typename ConnectionListType::iterator it = connection_list_.begin(); it != connection_list_.end(); it++ )
    {
      std::cout << "Dispatching to remote node: " << it->first << std::endl;
      it->second->dispatchToRemoteNode( it->first, topic, msg_s );
    }
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
