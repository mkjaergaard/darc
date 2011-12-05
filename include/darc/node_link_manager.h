#ifndef __DARC_NODE_LINK_MANAGER_H_INCLUDED__
#define __DARC_NODE_LINK_MANAGER_H_INCLUDED__

#include <boost/xpressive/xpressive.hpp>
#include <darc/udp/link_manager.h>

namespace xp = boost::xpressive;

namespace darc
{

class NodeLinkManager
{
private:
  udp::LinkManager udp_manager_;
  const xp::sregex url_rex_;

  typedef std::map<const std::string, LinkManagerAbstract*> ManagerMapType;
  ManagerMapType manager_map_;

public:
  NodeLinkManager( boost::asio::io_service * io_service ) :
    udp_manager_( io_service ),
    url_rex_( (xp::s1= +xp::_w) >> "://" >> (xp::s2= +~xp::_n) )
  {
    manager_map_["udp"] = &udp_manager_;
  }

  NodeLink::Ptr accept( const std::string& url )
  {
    xp::smatch what;
    if( regex_match( url, what, url_rex_ ) )
    {
      LinkManagerAbstract * mngr = getManager(what[1]);
      if( mngr )
      {
	return mngr->accept(what[2]);
      }
      else
      {
	std::cout << "Unsupported protocol: " << what[1] << std::endl;
	return NodeLink::Ptr();
      }
    }
    else
    {
      std::cout << "Invalid URL: " << url << std::endl;
	return NodeLink::Ptr();
    }
  }

  NodeLink::Ptr connect( uint32_t remote_node_id, const std::string& url )
  {
    xp::smatch what;
    if( regex_match( url, what, url_rex_ ) )
    {
      LinkManagerAbstract * mngr = getManager(what[1]);
      if( mngr )
      {
	return mngr->connect(remote_node_id, what[2]);
	return NodeLink::Ptr();
      }
      else
      {
	std::cout << "Unsupported protocol: " << what[1] << std::endl;
	return NodeLink::Ptr();
      }
    }
    else
    {
      std::cout << "Invalid URL: " << url << std::endl;
      return NodeLink::Ptr();
    }

  }

private:
  LinkManagerAbstract * getManager(const std::string& protocol)
  {
    ManagerMapType::iterator elem = manager_map_.find(protocol);
    if( elem != manager_map_.end() )
    {
      return elem->second;
    }
    else
    {
      return 0;
    }
  }

};

}

#endif
