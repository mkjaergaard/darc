#ifndef __DARC_REMOTE_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_REMOTE_DISPATCH_HANDLER_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <darc/serialized_message.h>

namespace darc
{

class RemoteDispatchHandler
{
private:
  boost::asio::io_service * io_service_;

  // Remote Connections
  

public:
  RemoteDispatchHandler( boost::asio::io_service io_service ) :
    io_service_( &io_service )
  {
  }

  

  template<typename T>
  void handleRemoteDispatch( const boost::shared_ptr<const T>& msg )
  {
    // if( remote subscribers )
    io_service_->post( boost::bind(&RemoteDispatchHandler::serializeAndDispatch, this, msg) );
  }

  template<typename T>
  void serializeAndDispatch( const boost::shared_ptr<const T> msg )
  {
    SerializedMessage::Ptr msg_s( new SerializedMessage(msg) );
    // for all remote subscribers
    // remote link handler->dispatchToNode()
  }

};

}

#endif
