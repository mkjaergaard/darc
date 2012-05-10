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
 * DARC LinkManager class
 *
 * \author Morten Kjaergaard
 */

#include <darc/network/zmq/protocol_manager.h>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <darc/log.h>
#include <darc/network/invalid_url_exception.h>
#include <darc/network/zmq/shared_buffer_zmq.h>

using namespace boost::asio;

namespace darc
{
namespace network
{
namespace zeromq
{

ProtocolManager::ProtocolManager(boost::asio::io_service * io_service,
				 network::LinkManagerCallbackIF * callback) :
  network::ProtocolManagerBase(),
  network::InboundLink(callback),
  io_service_(io_service),
  context_(1),
  inbound_id_(ConnectionID::create()),
  subscriber_socket_(context_, ZMQ_SUB)
{
}

#include <unistd.h>

void ProtocolManager::sendPacket(const ConnectionID& outbound_id,
				 packet::Header::PayloadType type, const NodeID& recv_node_id,
				 SharedBuffer buffer, std::size_t data_len)
{
  // Create Header
  packet::Header header(callback_->getNodeID(), recv_node_id, type);
  boost::array<uint8_t, 512> * header_buffer = new boost::array<uint8_t, 512>;
  std::size_t header_length = header.write( header_buffer->data(), header_buffer->size() );

  zmq::message_t message1(header_buffer->data(), header_buffer->size(), &ProtocolManager::freeBoostBuffer);
  outbound_connection_list_[outbound_id]->send(message1, ZMQ_SNDMORE);

  SharedBuffer * buffer_dyn = new SharedBuffer(buffer);

  zmq::message_t message2(buffer_dyn->data(), data_len, &ProtocolManager::freeSharedBuffer, buffer_dyn);

  outbound_connection_list_[outbound_id]->send(message2);
}

const ID& ProtocolManager::accept(const std::string& protocol, const std::string& url )
{
  assert(protocol == "zmq+tcp");
  
  std::string zmq_url = std::string("tcp://").append(url);
  DARC_INFO("ZeroMQ accepting on: %s", zmq_url.c_str());
  subscriber_socket_.bind(zmq_url.c_str());
  subscriber_socket_.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  recv_thread_ = boost::thread(boost::bind(&ProtocolManager::work, this));
  return inbound_id_;
}

void ProtocolManager::connect(const std::string& protocol, const std::string& url)
{
  assert(protocol == "zmq+tcp");
  
  std::string zmq_url = std::string("tcp://").append(url);
  DARC_INFO("ZeroMQ connecting on: %s", zmq_url.c_str());

  SocketPtr publisher_socket = SocketPtr(new zmq::socket_t(context_, ZMQ_PUB));
  publisher_socket->connect(zmq_url.c_str());

  ConnectionID id = ID::create();
  outbound_connection_list_.insert(OutboundConnectionListType::value_type(id, publisher_socket));
  sendDiscover(id);
}

void ProtocolManager::work()
{

  while(1)
  {
    DARC_INFO("ZeroMQ Waiting");

    int64_t more;
    size_t more_size = sizeof(more);

    // Expect two part message
    SharedBufferZmqPtr message1 = SharedBufferZmq::create();
    SharedBufferZmqPtr message2 = SharedBufferZmq::create();
    
    subscriber_socket_.recv(message1->message());
    subscriber_socket_.getsockopt (ZMQ_RCVMORE, &more, &more_size);
    assert(more != 0);

    subscriber_socket_.recv(message2->message());
    subscriber_socket_.getsockopt (ZMQ_RCVMORE, &more, &more_size);
    assert(more == 0);
    
    DARC_INFO("ZeroMQ message of size %u + %u", message1->size(), message2->size());

    callback_->receiveHandler(inbound_id_,
			      this,
			      SharedBuffer(message1),
			      0);
  }

}

} // namespace udp
} // namespace network
} // namespace darc
