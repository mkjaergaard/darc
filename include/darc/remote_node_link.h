#ifndef __DARC_REMOTE_NODE_LINK_H_INCLUDED__
#define __DARC_REMOTE_NODE_LINK_H_INCLUDED__

#include <darc/serialized_message.h>

namespace darc
{
  
class RemoteNodeLink
{
public:
  virtual void dispatchToRemoteNode( int id, SerializedMessage::ConstPtr msg ) = 0;

};

}

#endif
