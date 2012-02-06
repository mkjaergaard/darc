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
#include <darc/udp/link_manager.h>
#include <darc/packet/header.h>

namespace darc
{
namespace network
{

class LinkManager
{
private:
  uint32_t node_id_;

  // Protocol handlers
  typedef std::map<const std::string, LinkManagerAbstract*> ManagerMapType;
  ManagerMapType manager_map_;
  // -
  udp::LinkManager udp_manager_;

  // List of links
  //  Connections (Outgoing)
  typedef std::map< uint32_t, LinkBase::Ptr > ConnectionListType;
  ConnectionListType connection_list_;
  //  Acceptors (Incoming)
  typedef std::vector< LinkBase::Ptr > AcceptorListType;
  AcceptorListType acceptor_list_;

  // Callbacks
  typedef boost::function< void(SharedBuffer, std::size_t) > PacketReceivedHandlerType;
  std::map< packet::Header::PayloadType, PacketReceivedHandlerType > packet_received_handlers_;

public:
  LinkManager( boost::asio::io_service * io_service ) :
    node_id_(0xFFFF),
    udp_manager_( io_service )
  {
    // Link protocol names and protocol handlers
    manager_map_["udp"] = &udp_manager_;
  }

  void sendPacket( packet::Header::PayloadType type, SharedBuffer buffer, std::size_t data_len )
  {
    // todo: right now we send to all nodes.... should add routing functionality instead
    for( ConnectionListType::iterator it = connection_list_.begin(); it != connection_list_.end(); it++ )
    {
      it->second->sendPacket( it->first, type, buffer, data_len );
    }
  }

  void registerPacketReceivedHandler( packet::Header::PayloadType type, PacketReceivedHandlerType handler )
  {
    packet_received_handlers_[type] = handler;
  }

  void accept( const std::string& url )
  {
    LinkBase::Ptr link = createFromAccept(url);
    acceptor_list_.push_back( link );
    //? link->setReceiveCallback( boost::bind(&RemoteDispatcherManager::receiveFromRemoteNode, this, _1, _2, _3) );
    link->setReceiveCallback( boost::bind(&LinkManager::receiveHandler, this, _1, _2) );
  }

  void connect( uint32_t remote_node_id, const std::string& url )
  {
    LinkBase::Ptr link = createFromConnect(remote_node_id, url);
    connection_list_[remote_node_id] = link;
    link->setNodeID( node_id_ );
  }

  // todo: create one automatically
  void setNodeID( uint32_t node_id )
  {
    node_id_ = node_id;
    // todo: update alle existing links
  }

  uint32_t getNodeID()
  {
    return node_id_;
  }

private:
  void receiveHandler( SharedBuffer buffer, std::size_t data_len )
  {
    packet::Header header;
    header.read( buffer.data(), data_len );
    buffer.addOffset( packet::Header::size() );
    data_len -= packet::Header::size();

    // Switch on packet type
    if (header.payload_type == packet::Header::MSG_PACKET)
    {
      packet_received_handlers_[packet::Header::MSG_PACKET]( buffer, data_len );
    }
    else
    {
      std::cout << "Unknown packet type received??" << std::endl;
    }
  }

  LinkBase::Ptr createFromAccept( const std::string& url )
  {
    boost::smatch what;
    if( boost::regex_match( url, what, boost::regex("^(.+)://(.+)$") ) )
    {
      LinkManagerAbstract * mngr = getManager(what[1]);
      if( mngr )
      {
	return mngr->accept(what[2]);
      }
      else
      {
	std::cout << "Unsupported protocol: " << what[1] << std::endl;
	return LinkBase::Ptr();
      }
    }
    else
    {
      std::cout << "Invalid URL: " << url << std::endl;
	return LinkBase::Ptr();
    }
  }

  LinkBase::Ptr createFromConnect( uint32_t remote_node_id, const std::string& url )
  {
    boost::smatch what;
    if( boost::regex_match( url, what, boost::regex("^(.+)://(.+)$") ) )
    {
      LinkManagerAbstract * mngr = getManager(what[1]);
      if( mngr )
      {
	return mngr->connect(remote_node_id, what[2]);
	return LinkBase::Ptr();
      }
      else
      {
	std::cout << "Unsupported protocol: " << what[1] << std::endl;
	return LinkBase::Ptr();
      }
    }
    else
    {
      std::cout << "Invalid URL: " << url << std::endl;
      return LinkBase::Ptr();
    }
  }

  // Get the correct protocol handler
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
}

#endif
