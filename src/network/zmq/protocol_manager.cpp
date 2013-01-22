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

#include <darc/network/zmq/protocol_manager.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <darc/network/zmq/zmq_buffer.hpp>
#include <darc/network/link_header_packet.hpp>

#include <iris/glog.hpp>

using namespace boost::asio;

namespace darc
{
namespace network
{
namespace zeromq
{

ProtocolManager::ProtocolManager(boost::asio::io_service& io_service, network_manager * manager, peer& p):
  network::ProtocolManagerBase(),
  network::inbound_link_base(manager, p),
  peer_(p),
  context_(new zmq::context_t(1)),
  inbound_id_(ConnectionID::create())
{
}

ProtocolManager::~ProtocolManager()
{
  // todo: fix this creation of a dummy buffer
  int d = 0;
  outbound_data<darc::serializer::boost_serializer, int> o_d(d);
  buffer::shared_buffer dummy_data = boost::make_shared<buffer::const_size_buffer>(1024);
  o_d.pack(dummy_data);

  slog<iris::Debug>("Sending DISCONNECT");
  send_packet_to_all(ID::null(), link_header_packet::DISCONNECT, dummy_data);
}

#include <unistd.h>

void ProtocolManager::sendPacket(const ConnectionID& outbound_id,
                                 const ID& dest_peer_id,
                                 const uint16_t packet_type,
                                 buffer::shared_buffer data)
{
  // Create the Link Header Packet: do it somewhere else Base Class
  //
  link_header_packet lhp;
  lhp.packet_type = packet_type;
  lhp.dest_peer_id = dest_peer_id;
  lhp.src_peer_id = peer_.id();
  outbound_data<darc::serializer::boost_serializer, link_header_packet> o_lhp(lhp);

  buffer::shared_buffer header_data = boost::make_shared<buffer::const_size_buffer>(1024); // todo

  o_lhp.pack(header_data);
  //
  buffer::shared_buffer * keep_alive1 = new buffer::shared_buffer(header_data);
  zmq::message_t message1((void*)header_data->data(),
                          1024,//data->len(),
                          &zmq_buffer::free_func,
                          keep_alive1);
  //

  buffer::shared_buffer * keep_alive2 = new buffer::shared_buffer(data);
  zmq::message_t message2((void*)data->data(),
                          1024*10,//data->len(),
                          &zmq_buffer::free_func,
                          keep_alive2);

  OutboundConnectionListType::iterator item = outbound_connection_list_.find(outbound_id);
  if(item != outbound_connection_list_.end())
  {
    item->second->send(message1, ZMQ_SNDMORE);
    item->second->send(message2);
  }
  else
  {
    slog<iris::Warning>("Attempting to send to unknown outbound connection",
			"outbound id", iris::arg<ID>(outbound_id));
  }
}

void ProtocolManager::send_packet_to_all(const ID& dest_peer_id,
					 const uint16_t packet_type,
                                         buffer::shared_buffer data)
{
  // todo, only create the packet once and send to all
  for(OutboundConnectionListType::iterator it = outbound_connection_list_.begin();
      it != outbound_connection_list_.end();
      it++)
  {
    sendPacket(it->first,
               dest_peer_id,
               packet_type,
               data);
  }

}

const ID& ProtocolManager::accept(const std::string& protocol, const std::string& url )
{
  assert(protocol == "zmq+tcp");

  std::string zmq_url = std::string("tcp://").append(url);

  boost::shared_ptr<zmq::socket_t> socket = boost::make_shared<zmq::socket_t>(boost::ref(*context_), ZMQ_SUB);

  socket->bind(zmq_url.c_str());
  socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);

  slog<iris::Info>("ZeroMQ accept",
                   "URL", iris::arg<std::string>(zmq_url));

  recv_thread_list_.push_back(boost::make_shared<boost::thread>(boost::bind(&ProtocolManager::work, this, socket)));
  return ID::null();
}

void ProtocolManager::connect(const std::string& protocol, const std::string& url)
{
  assert(protocol == "zmq+tcp");

  std::string zmq_url = std::string("tcp://").append(url);

  SocketPtr publisher_socket = SocketPtr(new ::zmq::socket_t(*context_, ZMQ_PUB));

  int linger_value = 2000;
  publisher_socket->setsockopt(ZMQ_LINGER, &linger_value, sizeof(linger_value));
  publisher_socket->connect(zmq_url.c_str());

  ConnectionID id = ID::create();
  outbound_connection_list_.insert(OutboundConnectionListType::value_type(id, publisher_socket));

  slog<iris::Info>("ZeroMQ connect",
                   "URL", iris::arg<std::string>(zmq_url.c_str()),
		   "Out-ID", iris::arg<ID>(id));

  sendDiscover(id);
}

void ProtocolManager::work(boost::shared_ptr<zmq::socket_t> socket)
{
  try
  {
    while(1)
    {
      slog<iris::Debug>("ZeroMQ Waiting");

      int64_t more;
      size_t more_size = sizeof(more);

      boost::shared_ptr<zmq_buffer> header_msg = boost::make_shared<zmq_buffer>();
      boost::shared_ptr<zmq_buffer> body_msg = boost::make_shared<zmq_buffer>();

      bool recv1 = socket->recv(header_msg.get());
      if(!recv1)
      {
        slog<iris::Warning>("ZeroMQ1 recv returned non-zero");
      }

      socket->getsockopt (ZMQ_RCVMORE, &more, &more_size);
      assert(more != 0);

      bool recv2 = socket->recv(body_msg.get());
      if(!recv2)
      {
        slog<iris::Warning>("ZeroMQ2 recv returned non-zero");
      }

      socket->getsockopt (ZMQ_RCVMORE, &more, &more_size);
      assert(more == 0);
      header_msg->update_buffer();
      body_msg->update_buffer();

      slog<iris::Debug>("ZeroMQ message",
                        "size1", iris::arg<int>(header_msg->size()),
                        "size2", iris::arg<int>(body_msg->size()));

      packet_received(header_msg, body_msg);
    }
  }
  catch(zmq::error_t& e)
  {
    if(e.num() == ETERM)
    {
      // Expect an ETERM (Happens when we are shutting down)
    }
    else if(e.num() == EINTR)
    {
      slog<iris::Error>("ZeroMQ EINT exception");
    }
    else
    {
      throw e; // todo: rethrow is not proper handling of other errors
    }
  }
  slog<iris::Info>("ZeroMQ Exiting work thread");

}

} // namespace udp
} // namespace network
} // namespace darc
