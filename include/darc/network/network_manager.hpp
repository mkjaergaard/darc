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
 * DARC NetworkManager class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <darc/peer.hpp>
#include <darc/id.hpp>
#include <darc/inbound_data.hpp>
#include <darc/serializer/boost.hpp>
#include <darc/network/protocol_manager_base.hpp>
#include <darc/network/link_header_packet.hpp>
#include <iris/glog.hpp>
#include <darc/id_arg.hpp>
#include <darc/network/zmq/protocol_manager.hpp>

namespace darc
{
namespace network
{

typedef ID NodeID;
typedef ID ConnectionID;

class network_manager
{
private:
  peer& peer_;

  // Protocol Managers
  zeromq::ProtocolManager zmq_manager_;

  // Map "protocol" -> Manager
  typedef std::map<const std::string, ProtocolManagerBase*> ManagerProtocolMapType;
  ManagerProtocolMapType manager_protocol_map_;

  // Map "Inbound ConnectionID" -> Manager
  typedef std::map<const ConnectionID, ProtocolManagerBase*> ManagerConnectionMapType;
  ManagerConnectionMapType manager_connection_map_;

  // Node -> Outbound connection map (handle this a little more intelligent, more connections per nodes, timeout etc)
  typedef std::map<const NodeID, const ConnectionID> NeighbourNodesType; // NodeID -> OutboundID
  NeighbourNodesType neighbour_nodes_;

public:
  network_manager(boost::asio::io_service &io_service, darc::peer& p) :
    peer_(p),
    zmq_manager_(io_service, this, p)
  {
    peer_.set_send_to_function(boost::bind(&network_manager::sendPacket, this, _1, _2));
    manager_protocol_map_["zmq+tcp"] = &zmq_manager_;
  }

  ~network_manager()
  {
    // disconnect all peers
  }

  void sendPacket(const NodeID& recv_node_id, buffer::shared_buffer data)
  {
    // ID::null means we send to all nodes
    if( recv_node_id == ID::null() )
    {
      for( NeighbourNodesType::iterator it = neighbour_nodes_.begin(); it != neighbour_nodes_.end(); it++ )
      {
        zmq_manager_.sendPacket(it->second, it->first, link_header_packet::SERVICE, data);
      }
    }
    else
    {
      NeighbourNodesType::iterator item = neighbour_nodes_.find(recv_node_id);
      if(item != neighbour_nodes_.end())
      {
        zmq_manager_.sendPacket(item->second, recv_node_id, link_header_packet::SERVICE, data);
      }
      else
      {
        iris::glog<iris::Warning>("network_manager: sending packet to unknown peer_id",
                                  "peer_id", iris::arg<ID>(recv_node_id));
      }
    }
  }

  void accept(const std::string& url)
  {
//    try
    {
      boost::smatch what;
      if(boost::regex_match( url, what, boost::regex("^(.+)://(.+)$") ))
      {
        ProtocolManagerBase * mngr = getManager(what[1]);
        if(mngr)
        {
          ConnectionID inbound_id = mngr->accept(what[1], what[2]);
          manager_connection_map_.insert(ManagerConnectionMapType::value_type(inbound_id, mngr));
        }
        else
        {
          iris::glog<iris::Error>("network_manager: unsupported protocol",
                                  "url", iris::arg<std::string>(url));
        }
      }
      else
      {
        iris::glog<iris::Error>("network_manager: invalid url",
                                "url", iris::arg<std::string>(url));
      }
    }
//    catch(std::exception& e) //todo: handle the possible exceptions
//    {
//      std::cout << e.what() << std::endl;
//    }
  }

  void connect(const std::string& url)
  {
//    try
    {
      boost::smatch what;
      if( boost::regex_match(url, what, boost::regex("^(.+)://(.+)$")) )
      {
        ProtocolManagerBase * mngr = getManager(what[1]);
        if(mngr)
        {
          mngr->connect(what[1], what[2]);
        }
        else
        {
          iris::glog<iris::Error>("network_manager: unsupported protocol",
                                  "url", iris::arg<std::string>(url));
        }
      }
      else
      {
        iris::glog<iris::Error>("network_manager: invalid url",
                                "url", iris::arg<std::string>(url));
      }
    }
//    catch(std::exception& e) //todo: handle the possible exceptions
//    {
//      std::cout << e.what() << std::endl;
//    }
  }

  void neighbour_peer_discovered(const ID& src_peer_id, const ID& connection_id)
  {
    //todo: check if it exists already
    neighbour_nodes_.insert(NeighbourNodesType::value_type(src_peer_id, connection_id));
    peer_.peer_connected(src_peer_id);
  }

  void neighbour_peer_disconnected(const ID& src_peer_id, const ID& connection_id)
  {
    // todo: verify we have the node
    neighbour_nodes_.erase(src_peer_id);
    peer_.peer_disconnected(src_peer_id);
  }

  void service_packet_received(const ID& src_peer_id, buffer::shared_buffer data)
  {
    peer_.recv(src_peer_id, data);
  }

private:
  // Get the protocol manager from a protocol name
  ProtocolManagerBase * getManager(const std::string& protocol)
  {
    ManagerProtocolMapType::iterator elem = manager_protocol_map_.find(protocol);
    if( elem != manager_protocol_map_.end() )
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
