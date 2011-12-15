#ifndef __DARC_NODE_LINK_H_INCLUDED__
#define __DARC_NODE_LINK_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <darc/serialized_message.h>
#include <darc/packet/header.h>

namespace darc
{
  
class NodeLink
{
protected:
  typedef boost::function<void (SharedBuffer, std::size_t)> ReceiveCallbackType;
  ReceiveCallbackType receive_callback_;
  uint32_t node_id_;

  NodeLink() : node_id_( 0xFFFFFF )
  {
  }

public:
  typedef boost::shared_ptr<NodeLink> Ptr;

  ~NodeLink()
  {
  }

  void setNodeID( uint32_t node_id )
  {
    node_id_ = node_id;
  }

  virtual void sendPacket( uint32_t remote_node_id, packet::Header::PayloadType type, SharedBuffer buffer, std::size_t data_len ) = 0;

  void setReceiveCallback( ReceiveCallbackType receive_callback )
  {
    receive_callback_ = receive_callback;
  }

};

}

#endif
