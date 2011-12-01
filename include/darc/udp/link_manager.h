#ifndef __DARC_UDP_LINK_MANAGER_H__
#define __DARC_UDP_LINK_MANAGER_H__

#include <boost/xpressive/xpressive.hpp>
#include <boost/lexical_cast.hpp>
#include <darc/node_link.h>
#include <darc/udp/link.h>

namespace xp = boost::xpressive;

namespace darc
{

// todo: put somewhere else
class LinkManagerAbstract
{
public:
  virtual ~LinkManagerAbstract() {}

  virtual void accept( const std::string& url ) = 0;
  virtual void connect( const std::string& url ) = 0;

};

namespace udp
{

class LinkManager : public darc::LinkManagerAbstract
{
private:
  boost::asio::io_service * io_service_;
  udp::Link::Ptr link_;
  const xp::sregex url_rex_;

public:
  LinkManager( boost::asio::io_service * io_service ) :
    io_service_(io_service),
    url_rex_( (xp::s1= +~xp::_n) >> ':' >> (xp::s2= +xp::_d) ) // hostname:port
  {
  }

  void accept( const std::string& url )
  {
    xp::smatch what;
    if( regex_match( url, what, url_rex_ ) )
    {
      if( link_.get() != 0 )
      {
	link_.reset( new udp::Link( io_service_, boost::lexical_cast<int>(what[2]) ) );
      }
      else
      {
        std::cout << "only one UDP acceptor allowed right now: " << std::endl;
      }
    }
    else
    {
      std::cout << "Invalid URL: " << url << std::endl;
    }
  }

  void connect( const std::string& url )
  {
    xp::smatch what;
    if( regex_match( url, what, url_rex_ ) )
    {
      if( link_.get() != 0 )
      {
	link_->addRemoteNode(1, what[1], what[2]);
      }
      else
      {
        std::cout << "Must create UDP acceptor before connecting" << std::endl;
      }
    }
    else
    {
      std::cout << "Invalid URL: " << url << std::endl;
    }

  }

};

} // namespace udp
} // namespace darc

#endif
