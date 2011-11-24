#ifndef __DARC_REMOTE_NODE_CALLBACK_H_INCLUDED__
#define __DARC_REMOTE_NODE_CALLBACK_H_INCLUDED__

#include <darc/serialized_message.h>

namespace darc
{
  
class RemoteNodeCallback
{
public:
  virtual void dispatchFromRemoteNode( int id, SerializedMessage::ConstPtr msg ) = 0;

};

}

#endif
