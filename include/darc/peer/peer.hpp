/*
 * Copyright (c) 2013, Prevas A/S
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
 * \author Morten Kjaergaard
 */

#pragma once

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/signal.hpp>
#include <darc/id.hpp>
#include <darc/network/outbound_data.hpp>
#include <darc/network/service_header_packet.hpp>
#include <darc/buffer/shared_buffer.hpp>

namespace darc
{

class peer_service;

class peer
{
public:
  typedef uint32_t service_type;

protected:
  typedef std::map<service_type, peer_service*> service_list_type;

  typedef boost::function<void(const darc::ID&, darc::buffer::shared_buffer)> send_to_function_type;

  typedef boost::signal<void(const ID&)> peer_connected_signal_type;
  typedef boost::signal<void(const ID&)> peer_disconnected_signal_type;

  send_to_function_type send_to_function_;
  ID id_;
  service_list_type service_list_;

  peer_connected_signal_type peer_connected_signal_;
  peer_disconnected_signal_type peer_disconnected_signal_;

public:
  peer() :
    id_(ID::create())
  {}

  void send_to(const ID& peer_id, service_type service, const outbound_data_base& data);
  void recv(const ID& src_peer_id, buffer::shared_buffer data);
  void attach(service_type service_index, peer_service * service_instance);

  void peer_connected(const ID& peer_id);
  void peer_disconnected(const ID& peer_id);

  virtual void set_send_to_function(send_to_function_type send_to_function)
  {
    send_to_function_ = send_to_function;
  }

  const ID& id()
  {
    return id_;
  }

  peer_connected_signal_type& peer_connected_signal()
  {
    return peer_connected_signal_;
  }

  peer_disconnected_signal_type& peer_disconnected_signal()
  {
    return peer_disconnected_signal_;
  }

};

}
