#ifndef __DARC_NODE_H_INCLUDED__
#define __DARC_NODE_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <darc/publish/local_dispatcher_manager.h>
#include <darc/procedure/local_dispatcher_manager.h>

namespace darc
{

class Node
{
public:
  typedef boost::shared_ptr<Node> Ptr;
  
public:
  virtual void run() = 0;
  virtual publish::LocalDispatcherManager& getPublisherManager() = 0;
  virtual procedure::LocalDispatcherManager& getProcedureManager() = 0;
  virtual void accept( const std::string& url ) = 0;
  virtual void connect( uint32_t remote_node_id, const std::string& url ) = 0;
  virtual void setNodeID( uint32_t node_id ) = 0;

  static Node::Ptr create();
};

}

#endif
