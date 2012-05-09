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
 * DARC Link class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_NETWORK_UDP_LINK_H_INCLUDED__
#define __DARC_NETWORK_UDP_LINK_H_INCLUDED__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <darc/log.h>
#include <darc/id.h>
#include <darc/shared_buffer.h>
#include <darc/network/packet/header.h>
#include <darc/network/packet/discover.h>
#include <darc/network/packet/discover_reply.h>
#include <darc/network/inbound_link.h>

namespace darc
{
namespace network
{
namespace udp
{

class Link : public darc::network::InboundLink
{
private:
  boost::asio::io_service * io_service_;

  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint received_from_endpoint_;
  bool opened_;

  ID inbound_id_;

  typedef std::map<const ID, const boost::asio::ip::udp::endpoint> OutboundConnectionListType;
  OutboundConnectionListType outbound_connection_list_;

public:
  Link(network::LinkManagerCallbackIF * callback, boost::asio::io_service * io_service):
    InboundLink(callback),
    io_service_(io_service),
    socket_(*io_service),
    opened_(false),
    inbound_id_(ID::create())
  {
    socket_.open(boost::asio::ip::udp::v4()); //throws boost::system::system_error

    //boost::asio::socket_base::reuse_address option(true); // stuff for multicasting
    //socket_.set_option(option);
  }

  bool bind(const boost::asio::ip::udp::endpoint& local_endpoint)
  {
    boost::system::error_code ec;
    socket_.bind(local_endpoint, ec);
    if(!ec)
    {
      opened_ = true;
      startReceive();
      return true;
    }
    return false;
  }

  const ID& getInboundID()
  {
    return inbound_id_;
  }

  void startReceive()
  {
    SharedBuffer recv_buffer = SharedBufferArray::create(4098);
    socket_.async_receive_from( boost::asio::buffer(recv_buffer.data(), recv_buffer.size()), received_from_endpoint_,
                                boost::bind(&Link::handleReceive, this,
				recv_buffer,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
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

  void sendDiscover(const ID& outbound_id)
  {
    DARC_INFO("Outbound %s", outbound_id.short_string().c_str());
    std::size_t data_len = 1024*32;
    SharedBuffer buffer = SharedBufferArray::create(data_len);

    // Create packet
    network::packet::Discover discover(outbound_id);
    std::size_t len = discover.write( buffer.data(), buffer.size() );
    sendPacket( outbound_id, network::packet::Header::DISCOVER_PACKET, ID::null(), buffer, len );
  }

  void sendDiscoverReply(const ID& remote_outbound_id, const ID& remote_node_id)
  {
    // Create packet
    std::size_t data_len = 1024*32;
    SharedBuffer buffer = SharedBufferArray::create(data_len);
    network::packet::DiscoverReply discover_reply(remote_outbound_id);
    std::size_t len = discover_reply.write( buffer.data(), buffer.size() );

    // Send packet
    sendPacketToAll(network::packet::Header::DISCOVER_REPLY_PACKET, remote_node_id, buffer, len );
  }

  const ID& addOutboundConnection(const boost::asio::ip::udp::endpoint& remote_endpoint)
  {
    ID outbound_id = ID::create();
    OutboundConnectionListType::iterator elem =
      outbound_connection_list_.insert( OutboundConnectionListType::value_type(outbound_id, remote_endpoint) ).first;

    return elem->first;
  }

  //todo do this better (e.g. reuse buffer)
  void sendPacketToAll(packet::Header::PayloadType type, const ID& recv_node_id, SharedBuffer buffer, std::size_t data_len)
  {
    for( OutboundConnectionListType::iterator it = outbound_connection_list_.begin();
	 it != outbound_connection_list_.end();
	 it++ )
    {
      sendPacket(it->first, type, recv_node_id, buffer, data_len);
    }
  }

  void sendPacket(const ID& outbound_id, packet::Header::PayloadType type, const ID& recv_node_id, SharedBuffer buffer, std::size_t data_len)
  {
    // Create Header
    packet::Header header(callback_->getNodeID(), recv_node_id, type);

    boost::array<uint8_t, 512> header_buffer;

    std::size_t header_length = header.write( header_buffer.data(), header_buffer.size() );

    boost::array<boost::asio::const_buffer, 2> combined_buffers = {{
	boost::asio::buffer(header_buffer.data(), header_length),
	boost::asio::buffer(buffer.data(), data_len)
      }};

    // todo: to do an async send_to, msg must be kept alive until the send is finished. How to do this?
    //       Impl a object fulfilling the boost buffer interface which holds the smart pointer internally....
    boost::system::error_code err;
    socket_.send_to(combined_buffers, outbound_connection_list_[outbound_id], 0, err);
    if(err)
    {
      DARC_FATAL("UDP send_to error. You should probably handle this.");
    }
  }

 public:
  void handleReceive(SharedBuffer recv_buffer, const boost::system::error_code& error, std::size_t size)
  {
    if ( error )
    {
      DARC_ERROR("UDP Receive Error: %s ", boost::system::system_error(error).what());
    }
    else
    {
      DARC_INFO("%s", inbound_id_.short_string().c_str());
      callback_->receiveHandler(inbound_id_, this, recv_buffer, size);
    }
    startReceive();
  }

};

typedef boost::shared_ptr<udp::Link> LinkPtr;

} // namespace udp
} // namespace network
} // namespace darc

#endif
