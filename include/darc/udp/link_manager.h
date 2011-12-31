/*
 * Copyright (c) 2011, Prevas A/S
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Prevas A/S nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * DARC LinkManager class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_UDP_LINK_MANAGER_H__
#define __DARC_UDP_LINK_MANAGER_H__

#include <boost/xpressive/xpressive.hpp>
#include <boost/lexical_cast.hpp>
#include <darc/node_link.h>
#include <darc/udp/link.h>

namespace xp = boost::xpressive;

namespace darc
{

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

  darc::NodeLink::Ptr accept( const std::string& url )
  {
    xp::smatch what;
    if( regex_match( url, what, url_rex_ ) )
    {
      if( link_.get() == 0 ) // only works for one acceptor
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
    return link_;
  }

  darc::NodeLink::Ptr connect( uint32_t remote_node_id, const std::string& url )
  {
    xp::smatch what;
    if( regex_match( url, what, url_rex_ ) )
    {
      if( link_.get() != 0 )
      {
	link_->addRemoteNode(remote_node_id, what[1], what[2]);
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
    return link_;
  }

};

} // namespace udp
} // namespace darc

#endif
