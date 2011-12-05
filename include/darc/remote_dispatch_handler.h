#ifndef __DARC_REMOTE_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_REMOTE_DISPATCH_HANDLER_H_INCLUDED__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <darc/serialized_message.h>
#include <darc/node_link.h>
#include <darc/node_link_manager.h>

namespace darc
{

class RemoteDispatchHandler
{
private:
  boost::asio::io_service * io_service_;
  uint32_t node_id_;
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
  RemoteDispatchHandler( boost::asio::io_service * io_service ) :
    io_service_( io_service ),
    node_id_(0xFFFF),
    link_manager_( io_service )
  {
  }

  void accept( const std::string& url )
  {
    NodeLink::Ptr link = link_manager_.accept(url);
    acceptor_list_.push_back( link );
    link->setReceiveCallback( boost::bind(&RemoteDispatchHandler::receiveFromRemoteNode, this, _1, _2, _3) );
  }

  void connect( uint32_t remote_node_id, const std::string& url )
  {
    NodeLink::Ptr link = link_manager_.connect(remote_node_id, url);
    connection_list_[remote_node_id] = link;
    link->setNodeID( node_id_ );
  }

  // todo: right now it is set manually
  void setNodeID( uint32_t node_id )
  {
    node_id_ = node_id;
    // todo: update alle existing links
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
    io_service_->post( boost::bind(&RemoteDispatchHandler::serializeAndDispatch<T>, this, topic, msg) );
  }

  void receiveFromRemoteNode( uint32_t remote_node_id, const std::string& topic, SerializedMessage::ConstPtr msg_s )
  {
    std::cout << "Received a message from node " << remote_node_id << " with topic " << topic << std::endl;
    // todo: check if we should route to other nodes
    local_dispatch_function_(topic, msg_s);    
  }

};

}

#endif
