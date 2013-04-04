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

#include <darc/peer/peer.hpp>
#include <darc/peer/peer_service.hpp>
#include <darc/buffer/const_size_buffer.hpp>
#include <darc/network/inbound_data.hpp>
#include <darc/serializer/boost.hpp>

#include <iris/glog.hpp>
#include <darc/id_arg.hpp>

namespace darc
{

void peer::recv(const ID& src_peer_id, buffer::shared_buffer data)
{
  inbound_data<darc::serializer::boost_serializer, service_header_packet> header_i(data);

  service_list_type::iterator item = service_list_.find(header_i.get().service_type);
  if(item != service_list_.end())
  {
    item->second->recv(src_peer_id, data);
  }
  else
  {
    iris::glog<iris::Warning>("Received Data for unknown service id",
                              "service", iris::arg<uint32_t>(header_i.get().service_type));
  }
}

void peer::send_to(const ID& peer_id, service_type service, const outbound_data_base& data)
{
  service_header_packet header;
  header.service_type = service;
  outbound_data<darc::serializer::boost_serializer, service_header_packet> o_header(header);

  outbound_pair o_merge(o_header, data);

  buffer::shared_buffer buffer = boost::make_shared<buffer::const_size_buffer>(1024*10); // todo
  o_merge.pack(buffer);

  send_to_function_(peer_id, buffer);
}

void peer::peer_connected(const ID& peer_id)
{
  iris::glog<iris::Info>("Peer Connected",
                         "peer_id", iris::arg<ID>(peer_id));

  peer_connected_signal_(peer_id);
}

void peer::peer_disconnected(const ID& peer_id)
{
  iris::glog<iris::Info>("Peer Disconnected",
                         "peer_id", iris::arg<ID>(peer_id));

  peer_disconnected_signal_(peer_id);
}

void peer::attach(service_type service_index, peer_service * service_instance)
{
  assert(service_list_.find(service_index) == service_list_.end());
  service_list_.insert(
    service_list_type::value_type(service_index, service_instance));
}

}
