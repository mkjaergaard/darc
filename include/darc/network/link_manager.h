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

#ifndef __DARC_NODE_LINK_MANAGER_H_INCLUDED__
#define __DARC_NODE_LINK_MANAGER_H_INCLUDED__

#include <boost/regex.hpp>
#include <darc/network/udp/protocol_manager.h>
#include <darc/network/packet/header.h>
#include <darc/network/packet/discover.h>
#include <darc/network/packet/discover_reply.h>

namespace darc
{
namespace network
{

class LinkManager
{
private:
  ID node_id_;

  // Protocol handlers
  typedef std::map<const std::string, ProtocolManagerBase*> ManagerMapType;
  ManagerMapType manager_map_;

  // -
  udp::ProtocolManager udp_manager_;

  // Callbacks
  typedef boost::function< void(SharedBuffer, std::size_t) > PacketReceivedHandlerType;
  std::map< packet::Header::PayloadType, PacketReceivedHandlerType > packet_received_handlers_;

public:
  LinkManager( boost::asio::io_service * io_service, ID& node_id ) :
    node_id_(node_id),
    udp_manager_(io_service, boost::bind(&LinkManager::receiveHandler, this, _1, _2, _3))
  {
    // Link protocol names and protocol handlers
    manager_map_["udp"] = &udp_manager_;
  }

  void sendPacket( packet::Header::PayloadType type, SharedBuffer buffer, std::size_t data_len )
  {
    /*
    // todo: right now we send to all nodes.... should add routing functionality instead
    for( ConnectionListType::iterator it = connection_list_.begin(); it != connection_list_.end(); it++ )
    {
      it->second->sendPacket( it->first, type, buffer, data_len );
    }
    */
  }

  void sendPacketOnOutboundConnection( const ID& outbound_id, packet::Header::PayloadType type, SharedBuffer buffer, std::size_t data_len )
  {
    // todo: here we should check which protocol manager is the right one to dispatch to
    udp_manager_.sendPacketOnOutboundConnection(outbound_id, node_id_, type, buffer, data_len);
  }

  void registerPacketReceivedHandler( packet::Header::PayloadType type, PacketReceivedHandlerType handler )
  {
    packet_received_handlers_[type] = handler;
  }

  void accept( const std::string& url )
  {
    createFromAccept(url);
  }

  void connect(const std::string& url)
  {
    ID connection_id = createFromConnect(url);
    sendDiscover(connection_id);
  }

private:
  void sendDiscover(const ID& outbound_id)
  {
    std::size_t data_len = 1024*32;
    SharedBuffer buffer = SharedBuffer::create(data_len);

    DARC_INFO("Sending DISCOVER for connection: %s", outbound_id.short_string().c_str());
    // Create packet
    network::packet::Discover discover(outbound_id);
    std::size_t len = discover.write( buffer.data(), buffer.size() );
    sendPacketOnOutboundConnection( outbound_id, network::packet::Header::DISCOVER_PACKET, buffer, data_len );
  }

  void sendDiscoverReply(const ID& inbound_id, const ID& remote_outbound_id)
  {
    std::size_t data_len = 1024*32;
    SharedBuffer buffer = SharedBuffer::create(data_len);

    DARC_INFO("Sending DISCOVER_REPLY for connection: %s", remote_outbound_id.short_string().c_str());
    // Create packet
    network::packet::DiscoverReply discover_reply(remote_outbound_id);
    std::size_t len = discover_reply.write( buffer.data(), buffer.size() );
    udp_manager_.sendPacketToInboundGroup( inbound_id, node_id_, network::packet::Header::DISCOVER_REPLY_PACKET, buffer, data_len );
  }

  void handleDiscoverPacket(const ID& inbound_id, SharedBuffer buffer, std::size_t data_len)
  {
    packet::Discover discover;
    discover.read(buffer.data(), data_len);
    sendDiscoverReply(inbound_id, discover.link_id);
    // Send reply
  }

  void receiveHandler( const ID& inbound_id, SharedBuffer buffer, std::size_t data_len )
  {
    packet::Header header;
    header.read( buffer.data(), data_len );
    buffer.addOffset( packet::Header::size() );
    data_len -= packet::Header::size();

    // Switch on packet type
    switch(header.payload_type)
    {
      case packet::Header::MSG_PACKET:
      {
	packet_received_handlers_[packet::Header::MSG_PACKET]( buffer, data_len );
	break;
      }
      case packet::Header::DISCOVER_PACKET:
      {
	DARC_INFO("DISCOVER_PACKET");
	handleDiscoverPacket(inbound_id, buffer, data_len);
	break;
      }
      case packet::Header::DISCOVER_REPLY_PACKET:
      {
	DARC_INFO("DISCOVER_REPLY_PACKET");
	break;
      }
      default:
      {
	DARC_WARNING("Unknown packet type received");
	break;
      }
    }
  }

  void createFromAccept(const std::string& url)
  {
    boost::smatch what;
    if( boost::regex_match( url, what, boost::regex("^(.+)://(.+)$") ) )
    {
      ProtocolManagerBase * mngr = getManager(what[1]);
      if( mngr )
      {
	mngr->accept(what[2]);
      }
      else
      {
	DARC_ERROR("Unsupported Protocol: %s in %s", std::string(what[1]).c_str(), url.c_str());
      }
    }
    else
    {
      DARC_ERROR("Invalid URL: %s", url.c_str());
    }
  }

  ID createFromConnect(const std::string& url)
  {
    boost::smatch what;
    if( boost::regex_match(url, what, boost::regex("^(.+)://(.+)$")) )
    {
      ProtocolManagerBase * mngr = getManager(what[1]);
      if( mngr )
      {
	return mngr->connect(what[2]);
      }
      else
      {
	DARC_ERROR("Unsupported Protocol: %s in %s", std::string(what[1]).c_str(), url.c_str());
      }
    }
    else
    {
      DARC_ERROR("Invalid URL: %s", url.c_str());
    }
    return nullID();
  }

  // Get the correct protocol handler
  ProtocolManagerBase * getManager(const std::string& protocol)
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
}

#endif
