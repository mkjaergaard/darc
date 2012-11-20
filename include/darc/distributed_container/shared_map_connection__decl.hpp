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

#pragma once

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <darc/id.hpp>

#include <darc/distributed_container/control_packet.hpp>
#include <darc/distributed_container/header_packet.hpp>
#include <darc/distributed_container/control_packet.hpp>
#include <darc/distributed_container/update_packet.hpp>
#include <darc/inbound_data.hpp>
#include <darc/outbound_data.hpp>

#include <darc/serializer/boost.hpp>
#include <darc/serializer/std_pair_serialize.hpp>

namespace darc
{
namespace distributed_container
{

template<typename Key, typename T>
class shared_map;

template<typename Key, typename T>
class connection
{

public:
  typedef std::pair<ID/*origin*/, T> entry_type;
  typedef std::map<Key, entry_type> list_type;

protected:
  typedef std::pair<Key, entry_type> transfer_type;

  ID remote_location_id_; // location we are connected to
  ID remote_instance_id_; // instance we are connected to
  container_manager * manager_;
  shared_map<Key, T> * parent_;

  uint32_t last_sent_index_;
  uint32_t last_received_index_;

  list_type list_;

public:
  connection(container_manager * manager,
             shared_map<Key, T> * parent,
             const ID& remote_location_id,
             const ID& remote_instance_id) :
    remote_location_id_(remote_location_id),
    remote_instance_id_(remote_instance_id),
    manager_(manager),
    parent_(parent),
    last_sent_index_(0),
    last_received_index_(0)
  {
  }

  void do_connect()
  {
    control_packet ctrl;
    ctrl.command = control_packet::connect;
    outbound_data<serializer::boost_serializer, control_packet> o_ctrl(ctrl);

    manager_->send_to_location(
      parent_->id(),
      remote_location_id_,
      remote_instance_id_,
      header_packet::control,
      o_ctrl);
  }

  // called when we have a new map entry to send to remote
  void increment(const ID& informer, // where we got the info from
                 const Key& key,
                 const ID& origin,
                 const T& value,
                 uint32_t state_index)
  {
    entry_type entry(origin, value);

    last_sent_index_ = state_index;
    if(informer != remote_instance_id_) // dont send to informer
    {
      update_packet update;
      update.start_index = state_index;
      update.end_index = state_index;
      update.type = update_packet::partial;
      update.num_entries = 1;

      outbound_data<serializer::boost_serializer, update_packet> o_update(update);

      transfer_type item(key, entry);
      outbound_data<serializer::boost_serializer, transfer_type> o_item(item);

      outbound_pair o_data(o_update, o_item);

      manager_->send_to_location(parent_->id(),
                                 remote_location_id_,
                                 remote_instance_id_,
                                 header_packet::update,
                                 o_data);
    }
  }

  void handle_update(const header_packet& header,
                     const update_packet& update,
                     buffer::shared_buffer data);

  void full_update(typename list_type::iterator begin,
                   typename list_type::iterator end,
                   uint32_t state_index)
  {
    if(begin != end)
    {
      update_packet update;
      update.start_index = 0;
      update.end_index = state_index;
      update.type = update_packet::complete;
      update.num_entries = 0;

      // todo: smarter iterator count
      for(typename list_type::iterator it = begin;
          it != end;
          it++)
      {
        ++update.num_entries;
      }

      outbound_data<serializer::boost_serializer, update_packet> o_update(update);

      outbound_list<serializer::boost_serializer, typename list_type::iterator> o_item(begin, end);

      outbound_pair o_data(o_update, o_item);

      manager_->send_to_location(parent_->id(),
                                 remote_location_id_,
                                 remote_instance_id_,
                                 header_packet::update,
                                 o_data);
    }
    last_sent_index_ = 0;
  }

  const ID& peer_id()
  {
    return remote_location_id_;
  }

};

}
}
