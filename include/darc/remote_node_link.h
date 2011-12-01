#ifndef __DARC_REMOTE_NODE_LINK_H_INCLUDED__
#define __DARC_REMOTE_NODE_LINK_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <darc/serialized_message.h>

namespace darc
{
  
class RemoteNodeLink
{
protected:
  typedef boost::function<void (int, SerializedMessage::ConstPtr)> ReceiveCallbackType;
  ReceiveCallbackType receive_callback_;

public:
  typedef boost::shared_ptr<RemoteNodeLink> Ptr;

  virtual void dispatchToRemoteNode( int id, SerializedMessage::ConstPtr msg ) = 0;

  void setReceiveCallback( ReceiveCallbackType receive_callback )
  {
    receive_callback_ = receive_callback;
  }

};

}

#endif
