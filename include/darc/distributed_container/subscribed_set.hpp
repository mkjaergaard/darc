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

#include <set>
#include <boost/serialization/set.hpp>

#include <darc/distributed_container/container_base.hpp>
#include <darc/fsm.hpp>
#include <darc/outbound_data.hpp>
#include <darc/inbound_data.hpp>
#include <darc/serializer/boost.hpp>

#include <llog/logger.hpp>

namespace darc
{
namespace distributed_container
{

template<typename T>
class subscribed_set : public fsm<subscribed_set<T> >, public container_base
{
protected:
  typedef fsm<subscribed_set<T> > base;
  friend class fsm<subscribed_set<T> >;

  typedef std::set<T> list_type;
  typedef std::pair<typename std::set<T>::iterator, bool> insert_type;

  //******
  // Stuff
  list_type list_;

  ID server_location_id_;
  ID server_instance_id_;

  //**********
  // Events
  //**********
  struct ev_update_complete
  {
    update_packet u;
    darc::buffer::shared_buffer data;
  };

  struct ev_update_partial
  {
    bool x;
    update_packet u;
    darc::buffer::shared_buffer data;
  };

  struct ev_sub_req
  {
    ID location_id;
    ID instance_id;
  };

  //**********
  // States
  //**********
  typedef state<0> UNINIT;
  typedef state<1> PEND_SUBSCRIPTION;
  typedef state<2> UPDATED;
  typedef state<3> PEND_COMPLETE;

  //**********
  // Actions
  //**********
  void request_subscription(const ev_sub_req& e)
  {
    control_packet ctrl;
    ctrl.command = control_packet::subscribe;
    ctrl.client_id = id_;

    outbound_data<serializer::boost_serializer, control_packet> o_ctrl(ctrl);

    server_location_id_ = e.location_id;
    server_instance_id_ = e.instance_id;

    send_to_location(e.location_id,
                     e.instance_id,
                     header_packet::control,
                     o_ctrl);
  }

  void request_full_update()
  {
    control_packet ctrl;
    ctrl.command = control_packet::resend;
    ctrl.client_id = id_;

    outbound_data<serializer::boost_serializer, control_packet> o_ctrl(ctrl);

    send_to_location(server_location_id_,
                     server_instance_id_,
                     header_packet::control,
                     o_ctrl);
  }

  void populate_partial_update(ev_update_partial& e)
  {
    inbound_data<serializer::boost_serializer, list_type> i_added_list(e.data);
    inbound_data<serializer::boost_serializer, list_type> i_removed_list(e.data);

    for(typename list_type::iterator it = i_added_list.get().begin();
        it != i_added_list.get().end();
        it++)
    {
      insert_type result = list_.insert(*it);
      if(result.second)
      {
        llog::llog<llog::Severity::Debug>("New Item Callback");
      }
    }

    for(typename list_type::iterator it = i_removed_list.get().begin();
        it != i_removed_list.get().end();
        it++)
    {
      if(list_.erase(*it) > 0)
      {
        llog::llog<llog::Severity::Debug>("Erased Item Callback");
      }
    }

  }

  void populate_complete_set(ev_update_complete& e)
  {
    inbound_data<serializer::boost_serializer, list_type> i_added_list(e.data);

    list_.clear();

    for(typename list_type::iterator it = i_added_list.get().begin();
        it != i_added_list.get().end();
        it++)
    {
      insert_type result = list_.insert(*it);
      if(result.second)
      {
        llog::llog<llog::Severity::Debug>("New Item Callback 2");
      }
    }
  }

  void stash_partial_update(ev_update_partial& e)
  {
  }

  //**********
  // Default Handler
  //**********
  void handle(const UNINIT&,
              ev_sub_req& event)
  {
    base::trans(PEND_SUBSCRIPTION());
    request_subscription(event);
  }

  void handle(const PEND_SUBSCRIPTION&,
              ev_update_complete& event)
  {
    populate_complete_set(event);
    base::trans(UPDATED());
  }

  void handle(const UPDATED&,
              ev_update_partial& event)
  {
    populate_partial_update(event);
  }

  void handle(const PEND_COMPLETE&,
              ev_update_complete& event)
  {
    populate_complete_set(event);
    base::trans(UPDATED());
  }

  void handle(const PEND_COMPLETE&,
              ev_update_partial& event)
  {
    stash_partial_update(event);
  }

  template<typename State, typename Event>
  void handle(const State&, Event& event)
  {
    int state_copy = State::value;
    llog::llog<llog::Severity::Warning>(
      "Unhandled Event",
      "State", llog::Argument<int>(state_copy),
      "Event", llog::Argument<std::string>(typeid(Event).name()));
  }


public:
  //******
  // Public stuff
  void subscribe(const ID& loc, const ID& ins)
  {
    llog::llog<llog::Severity::Trace>("subscribe");

    ev_sub_req e;
    e.location_id = loc;
    e.instance_id = ins;

    base::post_event(e);
  }

  void recv(const header_packet& hdr, buffer::shared_buffer data)
  {
    assert(hdr.payload_type == header_packet::update);

    inbound_data<serializer::boost_serializer, update_packet> i_update(data);

    llog::llog<llog::Severity::Trace>(
      "recv",
      "type", llog::Argument<int>(i_update.get().type),
      "num", llog::Argument<int>(i_update.get().num_entries));

    switch(i_update.get().type)
    {
    case update_packet::complete:
    {
      ev_update_complete e;
      e.u = i_update.get();
      e.data = data;
      base::post_event(e);
    }
    break;
    case update_packet::partial:
    {
      ev_update_partial e;
      e.u = i_update.get();
      e.data = data;
      base::post_event(e);
    }
    break;
    }
  }

};

}
}
