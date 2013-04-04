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

#include <darc/network/zmq/zmq_listen_worker.hpp>
#include <darc/network/zmq/zmq_protocol_manager.hpp>
#include <darc/network/network_manager.hpp>

namespace darc
{
namespace network
{
namespace zeromq
{

zmq_listen_worker::zmq_listen_worker(zmq_protocol_manager * parent,
                                     const std::string& url,
                                     zmq::context_t& context) :
  zmq_worker(parent, context, ZMQ_XPUB),
  id_(darc::ID::create())
{
  int linger_value = 0;
  socket_.setsockopt(ZMQ_LINGER, &linger_value, sizeof(linger_value));
  socket_.bind(url.c_str());

  slog<iris::Info>("ZeroMQ accept",
                   "URL", iris::arg<std::string>(url),
                   "Out-ID", iris::arg<ID>(id_));
  run();
}

void zmq_listen_worker::work_receive()
{
  zmq::message_t msg;
  assert(socket_.recv(&msg));

  int event = ((char*)(msg.data()))[0];
  ID topic;
  memcpy(topic.data, (char*)msg.data() + 1, ID::static_size());

  if(event == 0 && topic != ID::null())
  {
    parent_->network_manager()->neighbour_peer_disconnected(topic, id_);
  }
  else if(event == 1 && topic != ID::null())
  {
    parent_->network_manager()->neighbour_peer_discovered(topic, id_);
  };
}

} // namespace zeromq
} // namespace network
} // namespace darc
