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
#include <darc/network/link_manager_callback_if.h>

namespace darc
{
namespace network
{

class LinkManager : public LinkManagerCallbackIF
{
private:
  ID node_id_;

  // Protocol handlers
  typedef std::map<const std::string, ProtocolManagerBase*> ManagerMapType;
  ManagerMapType manager_map_;

  // Protocol Managers
  udp::ProtocolManager udp_manager_;

  // Node -> Outbound connection map (handle this a little more intelligent, more connections per nodes, timeout etc)
  typedef std::map<const ID, const ID> NearNodesType;
  NearNodesType near_nodes_;

  // Callbacks to handlers of certain packet types
  typedef boost::function< void(SharedBuffer, std::size_t) > PacketReceivedHandlerType;
  std::map< packet::Header::PayloadType, PacketReceivedHandlerType > packet_received_handlers_;

public:
  LinkManager( boost::asio::io_service * io_service, ID& node_id ) :
    node_id_(node_id),
    udp_manager_(io_service, this)
  {
    // Link protocol names and protocol handlers
    manager_map_["udp"] = &udp_manager_;
  }

  const ID& getNodeID()
  {
    return node_id_;
  }

  void sendPacket( packet::Header::PayloadType type, SharedBuffer buffer, std::size_t data_len )
  {
    // todo: right now we send to all nodes.... should add routing functionality instead
    for( NearNodesType::iterator it = near_nodes_.begin(); it != near_nodes_.end(); it++ )
    {
      udp_manager_.sendPacketOnOutboundConnection(it->second, type, buffer, data_len );
    }
  }

  void sendPacketOnOutboundConnection( const ID& outbound_id, packet::Header::PayloadType type, SharedBuffer buffer, std::size_t data_len )
  {
    // todo: here we should check which protocol manager is the right one to dispatch to
    udp_manager_.sendPacketOnOutboundConnection(outbound_id, type, buffer, data_len);
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
  }

private:
  void handleDiscoverPacket(const ID& sender_node_id, LinkBase * source_link, SharedBuffer buffer, std::size_t data_len)
  {
    packet::Discover discover;
    discover.read(buffer.data(), data_len);
    source_link->sendDiscoverReply(discover.link_id);
    // If we received a DISCOVER from a node we dont know that we have a direct link to, send a DISCOVER back
    if(near_nodes_.count(sender_node_id) == 0)
    {
      source_link->sendDiscoverToAll();
    }
  }

  void handleDiscoverReplyPacket(const ID& sender_node_id, const ID& inbound_id, SharedBuffer buffer, std::size_t data_len)
  {
    // todo: check that we have such inbound link!
    packet::DiscoverReply discover_reply;
    discover_reply.read(buffer.data(), data_len);
    DARC_INFO("Found Node %s on outbound connection %s", sender_node_id.short_string().c_str(), discover_reply.link_id.short_string().c_str() );
    near_nodes_.insert(NearNodesType::value_type(sender_node_id, discover_reply.link_id));
  }

  void receiveHandler( const ID& inbound_id, LinkBase * source_link, SharedBuffer buffer, std::size_t data_len )
  {
    packet::Header header;
    header.read( buffer.data(), data_len );
    buffer.addOffset( packet::Header::size() );
    data_len -= packet::Header::size();

    // Discard packages from self, e.g. due to multicasting
    if(header.sender_node_id == getNodeID())
    {
      return;
    }

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
	handleDiscoverPacket(header.sender_node_id, source_link, buffer, data_len);
	break;
      }
      case packet::Header::DISCOVER_REPLY_PACKET:
      {
	DARC_INFO("DISCOVER_REPLY_PACKET");
	handleDiscoverReplyPacket(header.sender_node_id, inbound_id, buffer, data_len);
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
