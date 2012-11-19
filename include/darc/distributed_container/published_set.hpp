#pragma once

#include <set>
#include <map>
#include <boost/serialization/set.hpp>

#include <darc/distributed_container/container_base.hpp>
#include <darc/distributed_container/header_packet.hpp>
#include <darc/distributed_container/control_packet.hpp>
#include <darc/distributed_container/update_packet.hpp>
#include <darc/serializer/boost.hpp>
#include <darc/outbound_data.hpp>
#include <darc/inbound_data.hpp>

#include <llog/logger.hpp>

namespace darc
{
namespace distributed_container
{

template<typename T>
class published_set : public container_base
{
protected:
  typedef std::set<T> list_type;
  typedef std::pair<typename std::set<T>::iterator, bool> insert_type;
  typedef std::set<ID> subscriber_list_type;

  list_type list_;
  list_type added_list_;
  list_type removed_list_;

  uint32_t state_index_;
  uint32_t last_update_sent_;

  subscriber_list_type subscriber_list_;

  ID id_;

public:
  published_set() :
    state_index_(0),
    last_update_sent_(0)
  {
  }

  void insert(const T& item)
  {
    insert_type result = list_.insert(item);
    if(result.second)
    {
      if(removed_list_.erase(item) == 0)
      {
        added_list_.insert(item);
      }
      state_index_++;
    }
  }

  void erase(const T& item)
  {
    if(list_.erase(item) > 0)
    {
      if(added_list_.erase(item) == 0)
      {
        removed_list_.insert(item);
      }
      state_index_++;
    }
  }

  void flush()
  {
    if(state_index_ > last_update_sent_)
    {
      for(subscriber_list_type::iterator it = subscriber_list_.begin();
          it != subscriber_list_.end();
          it++)
      {
        send_updates(*it);
      }
      last_update_sent_ = state_index_;
      added_list_.clear();
      removed_list_.clear();
    }
  }

protected:
  void send_all(const ID& client_id)
  {
    update_packet msg;
    msg.type = update_packet::complete;
    msg.num_entries = list_.size();
    msg.start_index = 0;
    msg.end_index = state_index_;

    outbound_data<serializer::boost_serializer, update_packet> o_msg(msg);
    outbound_data<serializer::boost_serializer, list_type> o_list(list_);
    outbound_pair o_pair(o_msg, o_list);

    send_to_instance(client_id, header_packet::update, o_pair);
  }

  void send_updates(const ID& client_id)
  {
    update_packet msg;
    msg.type = update_packet::partial;
    msg.num_entries = list_.size();
    msg.start_index = last_update_sent_;
    msg.end_index = state_index_;

    outbound_data<serializer::boost_serializer, update_packet> o_msg(msg);
    outbound_data<serializer::boost_serializer, list_type> o_list1(added_list_);
    outbound_data<serializer::boost_serializer, list_type> o_list2(removed_list_);
    outbound_pair o_list(o_list1, o_list2);
    outbound_pair o_pair(o_msg, o_list);

    send_to_instance(client_id, header_packet::update, o_pair);
  }

  void recv(const header_packet& hdr, darc::buffer::shared_buffer data)
  {
    assert(hdr.payload_type == header_packet::control);

    inbound_data<serializer::boost_serializer, control_packet> i_control(data);

    handle_control_message(i_control.get());
  }

  void handle_control_message(const control_packet& msg)
  {
    llog::llog<llog::Severity::Trace>(
      "DistributedVector",
      "Command", llog::Argument<int>(msg.command));

    switch(msg.command)
    {
    case control_packet::subscribe:
      subscriber_list_.insert(msg.client_id);
      send_all(msg.client_id);
      break;
    case control_packet::unsubscribe:
      subscriber_list_.erase(msg.client_id);
      break;
    case control_packet::resend:
      if(subscriber_list_.find(msg.client_id) != subscriber_list_.end())
      {
        send_all(msg.client_id);
      }
      break;
    default:
      break;
    }
  }

};

}
}
