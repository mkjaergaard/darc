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
#include <darc/peer/peer.hpp>
#include <darc/id.hpp>
#include <darc/serializer/boost.hpp>
#include <darc/network/inbound_data.hpp>
#include <darc/network/protocol_manager_base.hpp>
#include <darc/network/link_header_packet.hpp>
#include <iris/glog.hpp>
#include <darc/id_arg.hpp>

namespace darc
{
namespace network
{

class network_manager
{
private:
  peer& peer_;

  // Map "protocol" string -> Manager
  typedef std::map<const std::string, boost::shared_ptr<protocol_manager_base> > ManagerProtocolMapType;
  ManagerProtocolMapType manager_protocol_map_;

  // Map "Inbound ConnectionID" -> Manager
  typedef std::map<const darc::ID, protocol_manager_base*> ManagerConnectionMapType;
  ManagerConnectionMapType manager_connection_map_;

  // Node -> Outbound connection map (handle this a little more intelligent, more connections per nodes, timeout etc)
  typedef std::map<const darc::ID, const darc::ID> NeighbourNodesType; // NodeID -> OutboundID
  NeighbourNodesType neighbour_nodes_;

public:
  network_manager(boost::asio::io_service &io_service, darc::peer& p);
  ~network_manager() {}

  void sendPacket(const darc::ID& recv_node_id, buffer::shared_buffer data);
  void accept(const std::string& url);
  void connect(const std::string& url);

  void neighbour_peer_discovered(const ID& src_peer_id, const ID& connection_id);
  void neighbour_peer_disconnected(const ID& src_peer_id, const ID& connection_id);
  void service_packet_received(const ID& src_peer_id, buffer::shared_buffer data);

private:
  // Get the protocol manager from a protocol name
  boost::shared_ptr<protocol_manager_base>& getManager(const std::string& protocol);

};

}
}
