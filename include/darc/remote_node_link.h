#ifndef __DARC_REMOTE_NODE_LINK_H_INCLUDED__
#define __DARC_REMOTE_NODE_LINK_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <darc/serialized_message.h>

namespace darc
{
  
class RemoteNodeLink
{
public:
  typedef boost::shared_ptr<RemoteNodeLink> Ptr;

  virtual void dispatchToRemoteNode( int id, SerializedMessage::ConstPtr msg ) = 0;

};

}

#endif
