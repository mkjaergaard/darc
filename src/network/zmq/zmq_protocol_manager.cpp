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

#include <darc/network/zmq/zmq_protocol_manager.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <darc/network/zmq/zmq_buffer.hpp>
#include <darc/network/link_header_packet.hpp>
#include <darc/network/network_manager.hpp>

#include <iris/glog.hpp>

using namespace boost::asio;

namespace darc
{
namespace network
{
namespace zeromq
{

zmq_protocol_manager::zmq_protocol_manager(class network_manager * manager,
                         peer& p):
  network::protocol_manager_base(),
  network::inbound_link_base(manager, p),
  peer_(p),
  context_(new zmq::context_t(1))
{
}

zmq_protocol_manager::~zmq_protocol_manager()
{
}

void zmq_protocol_manager::send_packet(const darc::ID& outbound_id,
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
  zmq::message_t topic_msg(dest_peer_id.size());
  memcpy(topic_msg.data(), dest_peer_id.data, dest_peer_id.size());

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

  listen_list_type::iterator item = listen_list_.find(outbound_id);
  if(item != listen_list_.end())
  {
    item->second->socket().send(topic_msg, ZMQ_SNDMORE);
    item->second->socket().send(message1, ZMQ_SNDMORE);
    item->second->socket().send(message2);
  }
  else
  {
    slog<iris::Warning>("Attempting to send to unknown outbound connection",
                        "outbound id", iris::arg<ID>(outbound_id));
  }
}

void zmq_protocol_manager::send_packet_to_all(const ID& dest_peer_id,
                                         const uint16_t packet_type,
                                         buffer::shared_buffer data)
{
  // todo, only create the packet once and send to all
  for(listen_list_type::iterator it = listen_list_.begin();
      it != listen_list_.end();
      it++)
  {
   send_packet(it->first,
               dest_peer_id,
               packet_type,
               data);
  }

}

void zmq_protocol_manager::accept(const std::string& protocol, const std::string& url )
{
  assert(protocol == "zmq+tcp");

  std::string zmq_url = std::string("tcp://").append(url);

  boost::shared_ptr<zmq_listen_worker> worker = boost::make_shared<zmq_listen_worker>(this,
                                                                                      zmq_url,
                                                                                      boost::ref(*context_));
  listen_list_.insert(listen_list_type::value_type(worker->id(), worker));
}

void zmq_protocol_manager::connect(const std::string& protocol, const std::string& url)
{
  assert(protocol == "zmq+tcp");

  std::string zmq_url = std::string("tcp://").append(url);

  boost::shared_ptr<zmq_connect_worker> worker = boost::make_shared<zmq_connect_worker>(this,
                                                                                        zmq_url,
                                                                                        boost::ref(*context_));
  connect_list_.push_back(worker);
}

} // namespace zeromq
} // namespace network
} // namespace darc
