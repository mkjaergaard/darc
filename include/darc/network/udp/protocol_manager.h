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

#ifndef __DARC_UDP_PROTOCOL_MANAGER_H_INCLUDED__
#define __DARC_UDP_PROTOCOL_MANAGER_H_INCLUDED__

#include <darc/network/protocol_manager_base.h>
#include <darc/network/udp/link.h>

namespace darc
{
namespace network
{
namespace udp
{

class ProtocolManager : public darc::network::ProtocolManagerBase
{
private:
  const static int DEFAULT_LISTEN_PORT = 58500;

  boost::asio::io_service * io_service_;
  boost::asio::ip::udp::resolver resolver_;

  typedef std::map<const ID, udp::LinkPtr> OutboundConnectionListType;
  typedef std::map<const ID, udp::LinkPtr> InboundConnectionListType;

  OutboundConnectionListType outbound_connection_list_;
  InboundConnectionListType inbound_connection_list_;
  udp::LinkPtr last_inbound_;

public:
  ProtocolManager(boost::asio::io_service * io_service, network::LinkManagerCallbackIF * callback);

  void sendPacket(const ID& outbound_id,
		  packet::Header::PayloadType type, const ID& recv_node_id,
		  SharedBuffer buffer, std::size_t data_len );

  const ID& accept(const std::string& protocol, const std::string& url);
  void connect(const std::string& protocol, const std::string& url);

private:
  const ID& accept_(boost::asio::ip::address &host, uint16_t port_begin, uint16_t port_end);
  void connect_(boost::asio::ip::address &host, uint16_t port_begin, uint16_t port_end);

  boost::asio::ip::udp::endpoint resolve(const std::string& host, const std::string& port);
  void createDefaultAcceptor();

};

} // namespace udp
} // namespace network
} // namespace darc

#endif
