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

#include <darc/fsm.hpp>
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

// fwd
template<typename Key, typename T>
class shared_map;




template<typename Key, typename T>
class connection : public fsm<connection<Key, T> >
{
  // FSM Definitions
protected:
  typedef fsm<connection<Key, T> > base;
  friend class fsm<connection<Key, T> >;

  typedef state<0> S_unconnected;
  typedef state<1> S_pending_complete;
  typedef state<2> S_connected;

  typedef event_0<0> E_connect;
  typedef event_0<1> E_connect_received;
  typedef event_2<2, update_packet, buffer::shared_buffer> E_complete_received;
  typedef event_2<3, update_packet, buffer::shared_buffer> E_partial_received;

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

  ~connection()
  {
    for(typename list_type::iterator it = list_.begin();
        it != list_.end();
        it++)
    {
      parent_->remove_(remote_instance_id_, //informer
                       it->first); // key
    }
  }

  // called when we have a new map entry to send to remote
  void insert(const ID& informer, // where we got the info from
                 const Key& key,
                 const ID& origin,
                 const T& value,
                 uint32_t state_index)
  {
    entry_type entry(origin, value);

    if(informer != remote_instance_id_) // dont send to informer
    {
      last_sent_index_ = state_index;

      update_packet update;
      update.start_index = state_index;
      update.end_index = state_index;
      update.type = update_packet::partial;
      update.num_entries_insert = 1;
      update.num_entries_remove = 0;

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

  // called when we have a new map entry to send to remote
  void remove(const ID& informer, // where we got the info from
              const Key& key,
              uint32_t state_index)
  {
    last_sent_index_ = state_index;
    if(informer != remote_instance_id_) // dont send to informer
    {
      update_packet update;
      update.start_index = state_index;
      update.end_index = state_index;
      update.type = update_packet::partial;
      update.num_entries_insert = 0;
      update.num_entries_remove = 1;

      outbound_data<serializer::boost_serializer, update_packet> o_update(update);

      outbound_data<serializer::boost_serializer, Key> o_item(key);

      outbound_pair o_data(o_update, o_item);

      manager_->send_to_location(parent_->id(),
                                 remote_location_id_,
                                 remote_instance_id_,
                                 header_packet::update,
                                 o_data);
    }
  }

  void send_full_update(typename list_type::iterator begin,
                        typename list_type::iterator end,
                        uint32_t state_index)
  {
    update_packet update;
    update.start_index = 0;
    update.end_index = state_index;
    update.type = update_packet::complete;
    update.num_entries_insert = 0;
    update.num_entries_remove = 0;

    // todo: smarter iterator count
    for(typename list_type::iterator it = begin;
        it != end;
        it++)
    {
      ++update.num_entries_insert;
    }

    outbound_data<serializer::boost_serializer, update_packet> o_update(update);

    outbound_list<serializer::boost_serializer, typename list_type::iterator> o_item(begin, end);

    outbound_pair o_data(o_update, o_item);

    manager_->send_to_location(parent_->id(),
                               remote_location_id_,
                               remote_instance_id_,
                               header_packet::update,
                               o_data);

    last_sent_index_ = state_index;
  }

  const ID& peer_id()
  {
    return remote_location_id_;
  }

public:
  // FSM Triggers
  void send_connect()
  {
    base::post_event(E_connect());
  }

  void handle_update(const header_packet& header,
                     const update_packet& update,
                     buffer::shared_buffer data)
  {
    if(update.type == update_packet::complete)
    {
      base::post_event(E_complete_received(update, data));
    }
    else
    {
      base::post_event(E_partial_received(update, data));
    }
  }

  void set_as_connected()
  {
    base::post_event(E_connect_received());
  }

protected:
  void insert_data(const update_packet& update,
                   buffer::shared_buffer data);

protected:
  // FSM Handlers
  void handle(const S_unconnected&,
              const E_connect& event)
  {
    base::trans(S_pending_complete());

    control_packet ctrl;
    ctrl.command = control_packet::connect;
    outbound_data<serializer::boost_serializer, control_packet> o_ctrl(ctrl);

    manager_->send_to_location(
      parent_->id(),
      remote_location_id_,
      remote_instance_id_,
      header_packet::control,
      o_ctrl);

    parent_->trigger_full_update(remote_instance_id_);
  }

  void handle(const S_unconnected&,
              const E_connect_received& event)
  {
    parent_->trigger_full_update(remote_instance_id_);
    base::trans(S_pending_complete());
  }

  void handle(const S_pending_complete&,
              const E_complete_received& event)
  {
    insert_data(event.a1_, event.a2_);
    base::trans(S_connected());
  }

  void handle(const S_connected&,
              const E_partial_received& event)
  {
    const update_packet& update = event.a1_;
    buffer::shared_buffer& data = (buffer::shared_buffer&)event.a2_; // todo: hack to remove const

    if(update.start_index != last_received_index_ + 1 &&
       update.start_index != last_sent_index_ + 1)
    {
      // todo: request full update
      iris::glog<iris::Fatal>("shared_map, incorrect index",
                              "set_id", iris::arg<ID>(parent_->id()),
                              "update.start_index", iris::arg<int>(update.start_index),
                              "last_received_index_ + 1", iris::arg<int>(last_received_index_ + 1),
                              "last_sent_index_ + 1", iris::arg<int>(last_sent_index_ + 1));
      assert(false);
    }

    last_received_index_ = update.end_index;
    insert_data(update, data);
  }

  template<typename State, typename Event>
  void handle(const State&, const Event&)
  {
    int state_copy = State::value;
    int event_copy = Event::value;
    iris::glog<iris::Warning>(
      "Unhandled Event",
      "Instance", iris::arg<ID>(parent_->id()),
      "State", iris::arg<int>(state_copy),
      "Event", iris::arg<int>(event_copy),
      "Type", iris::arg<std::string>(typeid(Event).name()));
  }

};

}
}
