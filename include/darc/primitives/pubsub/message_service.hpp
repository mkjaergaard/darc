#pragma once

#include <darc/primitives/pubsub/message_service__decl.hpp>

#include <boost/make_shared.hpp>
#include <darc/primitives/pubsub/local_dispatcher.hpp>

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
//  boost::mutex::scoped_lock lock(mutex_);
  tag_handle tag = nameserver_.register_tag(/*nameserver_.root(),*/ topic);
  local_dispatcher<T>* dispatcher = get_dispatcher<T>(tag);
  dispatcher->attach(publisher);
  return dispatcher;
}

template<typename T>
void message_service::detach(publisher_impl<T> &publisher, local_dispatcher<T>* dispatcher)
{
//  boost::mutex::scoped_lock lock(mutex_);
  dispatcher->detach(publisher);
}

template<typename T>
local_dispatcher<T>* message_service::attach(subscriber_impl<T> &subscriber,
                                             const std::string& topic)
{
//  boost::mutex::scoped_lock lock(mutex_);
  tag_handle tag = nameserver_.register_tag(/*nameserver_.root(),*/ topic);
  local_dispatcher<T>* dispatcher = get_dispatcher<T>(tag);
  dispatcher->attach(subscriber);
  return dispatcher;
}

template<typename T>
void message_service::detach(subscriber_impl<T> &subscriber, local_dispatcher<T>* dispatcher)
{
//  boost::mutex::scoped_lock lock(mutex_);
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

}
}
