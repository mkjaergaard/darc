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
#include <boost/signal.hpp>

#include <darc/id.hpp>

#include <darc/distributed_container/container_base.hpp>
#include <darc/distributed_container/container_manager.hpp>
#include <darc/distributed_container/control_packet.hpp>
#include <darc/distributed_container/header_packet.hpp>
#include <darc/distributed_container/control_packet.hpp>
#include <darc/distributed_container/update_packet.hpp>
#include <darc/inbound_data.hpp>
#include <darc/outbound_data.hpp>

#include <darc/distributed_container/shared_map_connection__decl.hpp>

#include <darc/serializer/boost.hpp>

#include <beam/static_scope.hpp>
#include <darc/id_arg.hpp>

namespace darc
{
namespace distributed_container
{

template<typename Key, typename T>
class shared_map : public container_base, public beam::static_scope<beam::Info>
{
  friend class connection<Key, T>;

protected:
  typedef connection<Key, T> connection_type;

public:
  typedef typename connection_type::entry_type entry_type;
  typedef typename connection_type::list_type list_type;
  typedef typename list_type::iterator iterator;

  boost::signal<void(const ID&, const ID&, const Key&, const T&)> signal_;

protected:
  typedef boost::shared_ptr<connection_type> connection_ptr;

  typedef std::map</*informer*/ID, connection_ptr> connection_list_type;
  connection_list_type connection_list_;

  list_type list_;
  uint32_t state_index_;


public:
  shared_map() :
    state_index_(0)
  {
  }

  ////////////////////////
  // Iterator Stuff
  iterator begin()
  {
    return list_.begin();
  }

  iterator end()
  {
    return list_.end();
  }

  const list_type& list() const
  {
    return list_;
  }

  list_type& list() // <- todo: list not updated through iterator
  {
    return list_;
  }
  //
  ////////////////////////

  ////////////////////////
  // Public interface
  void insert(const Key& key, const T& value)
  {
    insert_(id(), key, id(), value);
  }

  void connect(const ID& remote_location_id,
               const ID& remote_instance_id)
  {
    assert(connection_list_.find(remote_instance_id) == connection_list_.end());
    connection_ptr c = boost::make_shared<connection_type >(
      manager_,
      this,
      remote_location_id,
      remote_instance_id);
    connection_list_.insert(
      typename connection_list_type::value_type(remote_instance_id, c));
    c->send_connect();
    trigger_full_update(remote_instance_id);
  }
  //
  /////////////////////////

  const ID& get_peer_id(const ID& instance_id)
  {
    typename connection_list_type::iterator item = connection_list_.find(instance_id);
    assert(item != connection_list_.end());
    return item->second->peer_id();
  }

protected:
  // Insert entry into map and propagate to all other
  void insert_(const ID& informer,
               const Key& key,
               const ID& origin,
               const T& value)
  {
    slog<beam::Trace>("insert_",
                      "key", beam::arg<Key>(key),
                      "value", beam::arg<T>(value));

    entry_type entry(origin, value);
    list_.insert(
      typename list_type::value_type(key, entry));
    state_index_++;

    // Trigger signal for new entry
    signal_(id(), origin, key, value);

    // todo: Implement som sort of flush instead
    for(typename connection_list_type::iterator it = connection_list_.begin();
        it != connection_list_.end();
        it++)
    {
      it->second->insert(informer, key, origin, value, state_index_);
    }
  }

  void trigger_full_update(const ID& remote_instance_id)
  {
    typename connection_list_type::iterator item = connection_list_.find(remote_instance_id);
    assert(item != connection_list_.end());

    item->second->send_full_update(list_.begin(),
                                   list_.end(),
                                   state_index_);

  }

  void recv(const ID& src_location_id,
            const header_packet& hdr,
            darc::buffer::shared_buffer data)
  {
    switch(hdr.payload_type)
    {
    case header_packet::control:
    {
      inbound_data<serializer::boost_serializer, control_packet> i_control(data);
      handle_ctrl(src_location_id, hdr, i_control.get());
    }
    break;
    case header_packet::update:
    {
      inbound_data<serializer::boost_serializer, update_packet> i_update(data);
      handle_update(src_location_id, hdr, i_update.get(), data);
    }
    break;
    default:
    {
      assert(false);
      break;
    }
    }
  }

  void handle_ctrl(const ID& src_location_id,
                   const header_packet& header,
                   const control_packet& ctrl)
  {
    assert(ctrl.command == control_packet::connect);
    assert(connection_list_.find(header.src_instance_id) == connection_list_.end());

    connection_ptr c = boost::make_shared<connection_type>(
      manager_,
      this,
      src_location_id,
      header.src_instance_id);

    connection_list_.insert(
      typename connection_list_type::value_type(header.src_instance_id, c));

    c->set_as_connected();
  }

  void handle_update(const ID& src_location_id,
                     const header_packet& header,
                     const update_packet& update,
                     darc::buffer::shared_buffer data)
  {
    typename connection_list_type::iterator item = connection_list_.find(header.src_instance_id);
    assert(item != connection_list_.end());
    item->second->handle_update(header, update, data);
  }

};

}
}

#include <darc/distributed_container/shared_map_connection.hpp>
