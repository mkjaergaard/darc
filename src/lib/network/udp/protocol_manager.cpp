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

#include <darc/network/udp/protocol_manager.h>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <darc/log.h>
#include <darc/network/invalid_url_exception.h>

using namespace boost::asio;

namespace darc
{
namespace network
{
namespace udp
{

ProtocolManager::ProtocolManager(boost::asio::io_service * io_service, network::LinkManagerCallbackIF * callback) :
  network::ProtocolManagerBase(callback),
  io_service_(io_service),
  resolver_(*io_service)
{
}

boost::asio::ip::udp::endpoint ProtocolManager::resolve(const std::string& host, const std::string& port)
{
  // todo: do it async and handle errors and so on....
  boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, port);
  return *resolver_.resolve(query);
}

void ProtocolManager::createDefaultAcceptor()
{
  DARC_INFO("Accepting UDP on (ALL:%u) ", DEFAULT_LISTEN_PORT);
  LinkPtr connection(new udp::Link(callback_, io_service_) );
  inbound_connection_list_.insert( InboundConnectionListType::value_type(connection->getInboundID(), connection) );
  last_inbound_ = connection;
}

void ProtocolManager::sendPacket(const ID& outbound_id,
				 packet::Header::PayloadType type, const ID& recv_node_id,
				 SharedBuffer buffer, std::size_t data_len )
{
  OutboundConnectionListType::iterator item = outbound_connection_list_.find(outbound_id);
  if(item != outbound_connection_list_.end())
  {
    item->second->sendPacket(outbound_id, type, recv_node_id, buffer, data_len);
  }
  else
  {
    DARC_WARNING("UDPManager: sendPacket to unknown outbound_id: %s", outbound_id.short_string().c_str());
  }
}

const ID& ProtocolManager::accept(const std::string& protocol, const std::string& url)
{
  boost::smatch what;
  if( boost::regex_match( url, what, boost::regex("^(.+):(\\d+)$") ) ) //single port
  {
    ip::address host = ip::address::from_string(what[1]); //throws boost::system::system_error
    unsigned int port = boost::lexical_cast<unsigned int>(what[2]); //throws boost::bad_lexical_cast
    return accept_(host, port, port);
  }
  else if( boost::regex_match( url, what, boost::regex("^(.+):(\\d+)-(\\d+)$") ) ) //port range
  {
    ip::address host = ip::address::from_string(what[1]); //throws boost::system::system_error
    unsigned int port_begin = boost::lexical_cast<unsigned int>(what[2]); //throws boost::bad_lexical_cast
    unsigned int port_end = boost::lexical_cast<unsigned int>(what[3]); //throws boost::bad_lexical_cast
    return accept_(host, port_begin, port_end);
  }
  else
  {
    throw InvalidUrlException(url);
  }
}

void ProtocolManager::connect(const std::string& protocol, const std::string& url)
{
  boost::smatch what;
  if( boost::regex_match( url, what, boost::regex("^(.+):(|\\d+)$") ) ) //single port
  {
    ip::address host = ip::address::from_string(what[1]); //throws boost::system::system_error
    unsigned int port = boost::lexical_cast<unsigned int>(what[2]); //throws boost::bad_lexical_cast
    return connect_(host, port, port);
  }
  else if( boost::regex_match( url, what, boost::regex("^(.+):(\\d+)-(\\d+)$") ) ) //port range
  {
    ip::address host = ip::address::from_string(what[1]); //throws boost::system::system_error
    unsigned int port_begin = boost::lexical_cast<unsigned int>(what[2]); //throws boost::bad_lexical_cast
    unsigned int port_end = boost::lexical_cast<unsigned int>(what[3]); //throws boost::bad_lexical_cast
    return connect_(host, port_begin, port_end);
  }
  else
  {
    throw InvalidUrlException(url.c_str());
  }
}

void ProtocolManager::connect_(ip::address &host, uint16_t port_begin, uint16_t port_end)
{
  if( last_inbound_.get() == 0 )
  {
    createDefaultAcceptor();
  }
  // Allocate a connection ID
  for( uint16_t port = port_begin; port <= port_end; port++ )
  {
    const ID& outbound_id = last_inbound_->addOutboundConnection(ip::udp::endpoint(host, port));
    outbound_connection_list_.insert( OutboundConnectionListType::value_type(outbound_id, last_inbound_) );
    DARC_INFO("Connecting to UDP (%s:%s) (%s) ", host.to_string().c_str(), boost::lexical_cast<std::string>(port).c_str(), outbound_id.short_string().c_str() );
    last_inbound_->sendDiscover(outbound_id);
  }
}

const ID& ProtocolManager::accept_(ip::address &host, uint16_t port_begin, uint16_t port_end)
{
  LinkPtr connection = boost::make_shared<udp::Link>(callback_, io_service_);
  for(unsigned int port = port_begin; port <= port_end; port++)
  {
    if(connection->bind(ip::udp::endpoint(host, port)))
    {
      DARC_INFO("Accepting UDP on (%s:%s) ", host.to_string().c_str(), boost::lexical_cast<std::string>(port).c_str());
      inbound_connection_list_.insert( InboundConnectionListType::value_type(connection->getInboundID(), connection) );
      last_inbound_ = connection;
      return connection->getInboundID();
    }
  }
  throw darc::Exception("Unable to bind to port"); // todo: use other exception
}

} // namespace udp
} // namespace network
} // namespace darc
