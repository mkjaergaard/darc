#include <darc/node.h>
#include <darc/node_impl.h>

namespace darc
{
  Node::Ptr Node::create()
  {
    return Node::Ptr( new NodeImpl() );
  }
}
