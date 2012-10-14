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

#include <darc/serializer/boost.hpp>

#include <beam/static_scope.hpp>
#include <darc/id_arg.hpp>

namespace boost
{
namespace serialization
{

template<class Archive, class Key, class T>
void serialize(Archive & ar, std::pair<Key, T>& set, const unsigned int version)
{
  ar & set.first;
  ar & set.second;
}

}
}

namespace darc
{
namespace distributed_container
{

template<typename Key, typename T>
class shared_set;

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
  shared_set<Key, T> * parent_;

  uint32_t last_sent_index_;
  uint32_t last_received_index_;

  list_type list_;

public:
  connection(container_manager * manager,
	     shared_set<Key, T> * parent,
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

};

template<typename Key, typename T>
class shared_set : public container_base, public beam::static_scope<beam::Info>
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
  shared_set() :
    state_index_(0)
  {
  }

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

  list_type& list()
  {
    return list_;
  }

  void insert(const Key& key, const T& value)
  {
    remote_insert(id(), key, id(), value);
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
    c->do_connect();
    full_update(remote_instance_id);
  }

protected:
  void remote_insert(const ID& informer, //informer
		     const Key& key, // Key
		     const ID& origin, // origin
		     const T& value) // entry
  {
    slog<beam::Trace>("remote_insert",
		      "key", beam::arg<Key>(key),
		      "value", beam::arg<T>(value));

    entry_type entry(origin, value);
    list_.insert(
      typename list_type::value_type(key, entry));
    state_index_++;


    signal_(id(), origin, key, value);

    // do it in flush instead
    for(typename connection_list_type::iterator it = connection_list_.begin();
	it != connection_list_.end();
	it++)
    {
      it->second->increment(informer, key, origin, value, state_index_);
    }
  }

  void full_update(const ID& remote_instance_id)
  {
    typename connection_list_type::iterator item = connection_list_.find(remote_instance_id);
    assert(item != connection_list_.end());
    item->second->full_update(list_.begin(),
			      list_.end(),
			      state_index_);

  }

  void recv(const ID& src_location_id, const header_packet& hdr, darc::buffer::shared_buffer data)
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
    }
    }
  }

  void handle_ctrl(const ID& src_location_id, const header_packet& header, const control_packet& ctrl)
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
    full_update(header.src_instance_id);
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


// Connection impl
template<typename Key, typename T>
void connection<Key, T>::handle_update(const header_packet& header,
				       const update_packet& update,
				       buffer::shared_buffer data)
{
  if(update.type == update_packet::complete)
  {
    // todo: handle more intelligent since this might cause duplicate callbacks
    list_.clear();
  }
  else if(update.start_index != last_received_index_ + 1)
  {
    // todo: request full update
    assert(false);
  }
  last_received_index_ = update.end_index;

  for(size_t i = 0; i < update.num_entries; i++)
  {
    inbound_data<serializer::boost_serializer, transfer_type> i_item(data);
    typename list_type::value_type value(i_item.get().first, i_item.get().second);
    list_.insert(value);
    parent_->remote_insert(remote_instance_id_, //informer
			   value.first, // Key
			   value.second.first, // origin
			   value.second.second); // entry
  }
}

}
}
