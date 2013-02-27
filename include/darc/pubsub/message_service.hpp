#pragma once

#include <darc/pubsub/message_service__decl.hpp>

#include <boost/make_shared.hpp>
#include <darc/pubsub/local_dispatcher.hpp>

#include <darc/id.hpp>


typedef darc::ID IDType;

namespace darc
{
namespace pubsub
{

template<typename T>
local_dispatcher<T>* message_service::attach(publisher_impl<T> &publisher,
                                             const std::string& topic)
{
  boost::mutex::scoped_lock lock(mutex_);
  tag_handle tag = nameserver_.register_tag(/*nameserver_.root(),*/ topic);
  local_dispatcher<T>* dispatcher = get_dispatcher<T>(tag);
  dispatcher->attach(publisher);
  return dispatcher;
}

template<typename T>
void message_service::detach(publisher_impl<T> &publisher, local_dispatcher<T>* dispatcher)
{
  boost::mutex::scoped_lock lock(mutex_);
  dispatcher->detach(publisher);
}

template<typename T>
local_dispatcher<T>* message_service::attach(subscriber_impl<T> &subscriber,
                                             const std::string& topic)
{
  boost::mutex::scoped_lock lock(mutex_);
  tag_handle tag = nameserver_.register_tag(/*nameserver_.root(),*/ topic);
  local_dispatcher<T>* dispatcher = get_dispatcher<T>(tag);
  dispatcher->attach(subscriber);
  return dispatcher;
}

template<typename T>
void message_service::detach(subscriber_impl<T> &subscriber, local_dispatcher<T>* dispatcher)
{
  boost::mutex::scoped_lock lock(mutex_);
  dispatcher->detach(subscriber);
}

template<typename T>
local_dispatcher<T>* message_service::get_dispatcher(const tag_handle& tag)
{
  typename dispatcher_list_type::iterator elem = dispatcher_list_.find(tag->id());
  if(elem == dispatcher_list_.end())
  {
    boost::shared_ptr<local_dispatcher<T> > dispatcher
      = boost::make_shared<local_dispatcher<T> >(this, tag);

    dispatcher_list_.insert(
      typename dispatcher_list_type::value_type(tag->id(), dispatcher));

    return dispatcher.get();
  }
  else
  {
    boost::shared_ptr<local_dispatcher_base> &dispatcher_base = elem->second;
    // todo, try
    boost::shared_ptr<local_dispatcher<T> > dispatcher
      = boost::dynamic_pointer_cast<local_dispatcher<T> >(dispatcher_base);
    return dispatcher.get();
  }
}

///////////////////
// Network stuff
///////////////////
template<typename T>
void message_service::send_msg(const ID& tag_id, const ID& peer_id, const boost::shared_ptr<const T> &msg)
{
  payload_header_packet hdr;
  hdr.payload_type = message_packet::payload_id;
  outbound_data<serializer::boost_serializer, payload_header_packet> o_hdr(hdr);

  message_packet msg_hdr(tag_id);
  outbound_data<serializer::boost_serializer, message_packet> o_msg_hdr(msg_hdr);

  outbound_ptr<serializer::ros_serializer, T> o_msg(msg);

  outbound_pair o_pair1(o_hdr, o_msg_hdr);
  outbound_pair o_pair2(o_pair1, o_msg);

  send_to(peer_id, o_pair2);
}

template<typename T>
void message_service::dispatch_remotely(const ID& tag_id, const boost::shared_ptr<const T> &msg)
{
  send_msg(tag_id, ID::null(), msg);
  /*
  remote_list_type::iterator item = list_.find(tag_id);
  if(item != list_.end())
  {
    for(remote_tag_list_type::iterator it = item->second->begin();
        it != item->second->end();
        it++)
    {
      // todo: here we send a copy to all
      send_msg(/tag_id/it->second, it->first, msg);
    }
  }
  */
}

}
}
