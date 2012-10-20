/*
 * Copyright (c) 2012, Prevas A/S
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
 * DARC ZeroMQ LinkManager class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <zmq.hpp>
#include <boost/asio.hpp> // do we really need asio is this?
#include <boost/thread.hpp>
#include <darc/peer.hpp>
#include <darc/network/protocol_manager_base.hpp>
#include <darc/network/inbound_link_base.hpp>

namespace darc
{
namespace network
{

typedef ConnectionID ID;

class network_manager;

namespace zeromq
{

class ProtocolManager : public ProtocolManagerBase, public inbound_link_base
{
private:
  boost::asio::io_service * io_service_;
  peer& peer_;
  zmq::context_t context_;

  ConnectionID inbound_id_;
  zmq::socket_t subscriber_socket_;
  boost::thread recv_thread_;

  typedef boost::shared_ptr<zmq::socket_t> SocketPtr;
  typedef std::map<const ConnectionID, SocketPtr> OutboundConnectionListType;

  OutboundConnectionListType outbound_connection_list_;

public:
  ProtocolManager(boost::asio::io_service& io_service, network_manager * manager, peer& p);

  ~ProtocolManager()
  {
  }

  void sendPacket(const ConnectionID& outbound_id,
		  const ID& dest_peer_id,
		  const uint16_t packet_type,
		  buffer::shared_buffer data);

  virtual void send_packet_to_all(const uint16_t packet_type,
				  buffer::shared_buffer data);

  const ConnectionID& accept(const std::string& protocol, const std::string& url);
  void connect(const std::string& protocol, const std::string& url);

protected:
  void work();

};

} // namespace zeromq
} // namespace network
} // namespace darc
