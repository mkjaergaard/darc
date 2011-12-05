#ifndef __DARC_UDP_LINK_MANAGER_ABSTRACT_H__
#define __DARC_UDP_LINK_MANAGER_ABSTRACT_H__

#include <string>
#include <darc/node_link.h>

namespace darc
{

class LinkManagerAbstract
{
public:
  virtual ~LinkManagerAbstract() {}

  virtual darc::NodeLink::Ptr accept( const std::string& url ) = 0;
  virtual darc::NodeLink::Ptr connect( uint32_t remote_node_id, const std::string& url ) = 0;

};

} // namespace darc

#endif
