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

#ifndef __DARC_NETWORK_ZMQ_PROTOCOL_MANAGER_H_INCLUDED__
#define __DARC_NETWORK_ZMQ_PROTOCOL_MANAGER_H_INCLUDED__

#include <zmq.hpp>
#include <boost/asio.hpp> // do we really need asio is this?
#include <boost/thread.hpp>
#include <darc/network/protocol_manager_base.h>
#include <darc/network/inbound_link.h>
#include <darc/log.h>

namespace darc
{
namespace network
{
namespace zeromq
{

class ProtocolManager : public darc::network::ProtocolManagerBase, public darc::network::InboundLink
{
private:
  boost::asio::io_service * io_service_;
  zmq::context_t context_;

  ConnectionID inbound_id_;
  zmq::socket_t subscriber_socket_;
  boost::thread recv_thread_;

  typedef boost::shared_ptr<zmq::socket_t> SocketPtr;
  typedef std::map<const ConnectionID, SocketPtr> OutboundConnectionListType;

  OutboundConnectionListType outbound_connection_list_;

public:
  ProtocolManager(boost::asio::io_service * io_service, network::LinkManagerCallbackIF * callback);
  
  void sendPacket(const ConnectionID& outbound_id,
		  packet::Header::PayloadType type, const NodeID& recv_node_id,
		  SharedBuffer buffer, std::size_t data_len );

  const ConnectionID& accept(const std::string& protocol, const std::string& url);
  void connect(const std::string& protocol, const std::string& url);

  // **************
  // Impl of InboundLink, this could be done smarter
  void sendPacketToAll(packet::Header::PayloadType type, const ID& recv_node_id, SharedBuffer buffer, std::size_t data_len)
  {
    for(OutboundConnectionListType::iterator it = outbound_connection_list_.begin();
	it != outbound_connection_list_.end();
	it++ )
    {
      sendPacket(it->first, type, recv_node_id, buffer, data_len);
    }
  }

  void sendDiscoverToAll()
  {
    for(OutboundConnectionListType::iterator it = outbound_connection_list_.begin();
	it != outbound_connection_list_.end();
	it++)
    {
      sendDiscover(it->first);
    }
  }

private:
  void work();
  static void freeSharedBuffer(void * data, void * hint)
  {
    DARC_AUTOTRACE();
    SharedBuffer * buf = static_cast<SharedBuffer*>(hint);
    delete buf;
  }

  static void freeBoostBuffer(void * data, void * hint)
  {
    DARC_AUTOTRACE();
    boost::array<uint8_t, 512> * buf = static_cast<boost::array<uint8_t, 512>* >(hint);
    delete buf;
  }

  static void freeDummy(void * data, void * hint)
  {
    DARC_AUTOTRACE();
  }

};

} // namespace zeromq
} // namespace network
} // namespace darc

#endif
