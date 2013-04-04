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
 *
 *
 * \author Morten Kjaergaard
 */

#include <darc/distributed_container/container_manager.hpp>
#include <darc/distributed_container/container_base.hpp>
#include <darc/distributed_container/header_packet.hpp>
#include <darc/outbound_data.hpp>
#include <darc/inbound_data.hpp>
#include <darc/id_arg.hpp>

#include <boost/make_shared.hpp>

#include <iris/glog.hpp>

namespace darc
{
namespace distributed_container
{

container_manager::container_manager(darc::peer& p) :
  darc::peer_service(p, 15)
{
}

void container_manager::attach(container_base* entry)
{
  list_.insert(list_type::value_type(entry->id(), entry));
}

void container_manager::detatch(container_base* entry)
{
  list_.erase(entry->id());
}

void container_manager::recv(const ID& peer_id, buffer::shared_buffer data)
{
  inbound_data<serializer::boost_serializer, header_packet> i_hdr(data);

  instance_location_map_type::iterator item1 = instance_location_map.find(i_hdr.get().src_instance_id);
  if(item1 == instance_location_map.end())
  {
    instance_location_map.insert(
      instance_location_map_type::value_type(i_hdr.get().src_instance_id, peer_id));
  }

  list_type::iterator item = list_.find(i_hdr.get().dest_instance_id);
  if(item != list_.end())
  {
    iris::glog<iris::Trace>(
      "DistributedManager",
      "Data recv for", iris::arg<ID>(i_hdr.get().dest_instance_id));

    item->second->recv(peer_id, i_hdr.get(), data);
  }
  else
  {
    iris::glog<iris::Warning>(
      "DistributedManager: Data recv for unknown instance ID",
      "Data recv for", iris::arg<ID>(i_hdr.get().dest_instance_id));
  }
}

void container_manager::send_to_instance(const ID& src_instance_id,
                                         const ID& dest_instance_id,
                                         const uint32_t payload_type,
                                         const outbound_data_base& data)
{
  instance_location_map_type::iterator item = instance_location_map.find(dest_instance_id);
  if(item != instance_location_map.end())
  {
    send_to_location(src_instance_id,
                     item->second,
                     dest_instance_id,
                     payload_type,
                     data);
  }
  else
  {
    iris::glog<iris::Warning>(
      "DistributedManager: Unknown node",
      "InstanceID", iris::arg<ID>(dest_instance_id));
  }
}

void container_manager::send_to_location(const ID& src_instance_id,
                                         const ID& dest_location_id,
                                         const ID& dest_instance_id,
                                         const uint32_t payload_type,
                                         const outbound_data_base& data)
{
  iris::glog<iris::Trace>(
    "Send To Location",
    "Src Inst", iris::arg<ID>(src_instance_id),
    "Dst Loca", iris::arg<ID>(dest_location_id),
    "Dst Inst", iris::arg<ID>(dest_instance_id));

  header_packet hdr;
  hdr.src_instance_id = src_instance_id;
  hdr.dest_instance_id = dest_instance_id;
  hdr.payload_type = payload_type;

  outbound_data<serializer::boost_serializer, header_packet> o_hdr(hdr);
  outbound_pair o_pair(o_hdr, data);

  send_to(dest_location_id, o_pair);
}

}
}
