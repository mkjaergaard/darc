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
#include <boost/scoped_ptr.hpp>
#include <darc/peer/peer.hpp>
#include <darc/network/protocol_manager_base.hpp>
#include <darc/network/inbound_link_base.hpp>
#include <darc/network/zmq/zmq_listen_worker.hpp>
#include <darc/network/zmq/zmq_connect_worker.hpp>
#include <iris/static_scope.hpp>

namespace darc
{
namespace network
{

class network_manager; // fwd

namespace zeromq
{

class zmq_protocol_manager : public protocol_manager_base, public inbound_link_base
{
private:
  peer& peer_;

  typedef std::list<boost::shared_ptr<zmq_connect_worker> > connect_list_type;
  typedef std::map</*outbound*/ID, boost::shared_ptr<zmq_listen_worker> > listen_list_type;
  connect_list_type connect_list_;
  listen_list_type listen_list_;

  boost::scoped_ptr<zmq::context_t> context_;

public:
  zmq_protocol_manager(class network_manager * manager, peer& p);
  ~zmq_protocol_manager();

  void send_packet(const darc::ID& outbound_id,
                   const ID& dest_peer_id,
                   const uint16_t packet_type,
                   buffer::shared_buffer data);

  virtual void send_packet_to_all(const ID& dest_peer_id,
                                  const uint16_t packet_type,
                                  buffer::shared_buffer data);

  void accept(const std::string& protocol, const std::string& url);
  void connect(const std::string& protocol, const std::string& url);

  const darc::ID& peer_id()
  {
    return peer_.id();
  }

};

} // namespace zeromq
} // namespace network
} // namespace darc
