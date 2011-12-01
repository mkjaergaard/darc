#ifndef __DARC_REMOTE_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_REMOTE_DISPATCH_HANDLER_H_INCLUDED__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <darc/serialized_message.h>
#include <darc/node_link.h>

namespace darc
{

class RemoteDispatchHandler
{
private:
  boost::asio::io_service * io_service_;

  // Function to dispatch locally
  typedef boost::function<void (int, SerializedMessage::ConstPtr)> LocalDispatchFunctionType;
  LocalDispatchFunctionType local_dispatch_function_;

  // Remote Connections
  typedef std::map<int, NodeLink::Ptr > LinkListType;
  LinkListType link_list_;

public:
  RemoteDispatchHandler( boost::asio::io_service * io_service ) :
    io_service_( io_service )
  {
  }

  void setLocalDispatchFunction( LocalDispatchFunctionType local_dispatch_function )
  {
    local_dispatch_function_ = local_dispatch_function;
  }

  void addRemoteLink( int id, NodeLink::Ptr link )
  {
    link_list_[id] = link;
    link->setReceiveCallback( boost::bind(&RemoteDispatchHandler::receiveFromRemoteNode, this, _1, _2) );
  }

  // Triggered by asio post
  template<typename T>
  void serializeAndDispatch( const boost::shared_ptr<const T> msg )
  {
    SerializedMessage::Ptr msg_s( new SerializedMessage(msg) );
    // todo find the right nodes to dispatch to
    for( typename LinkListType::iterator it = link_list_.begin(); it != link_list_.end(); it++ )
    {
      std::cout << "Dispatching to remote node: " << it->first << std::endl;
      it->second->dispatchToRemoteNode( it->first, msg_s );
    }
  }

  // Called by LocalDispatcher
  template<typename T>
  void postRemoteDispatch( const boost::shared_ptr<const T> msg )
  {
    // if( remote subscribers )
    io_service_->post( boost::bind(&RemoteDispatchHandler::serializeAndDispatch<T>, this, msg) );
  }

  void receiveFromRemoteNode( int id, SerializedMessage::ConstPtr msg_s )
  {
    std::cout << "Received a message" << std::endl;
    // todo: check if we should route to other nodes
    local_dispatch_function_(id, msg_s);    
  }

};

}

#endif
