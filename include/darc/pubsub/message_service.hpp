#pragma once

#include <darc/pubsub/message_service__decl.hpp>

#include <boost/make_shared.hpp>
#include <darc/pubsub/dispatcher_group.hpp>

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
  tag_handle tag = nameserver_.register_tag(nameserver_.root(), topic);
  dispatcher_group<T>* group = get_dispatcher_group<T>(tag);
  local_dispatcher<T>* dispatcher = group->get_dispatcher(tag);
  tag->connect_new_tag_listener(boost::bind(&message_service::new_tag_event, this, _1, _2, _3));
  tag->connect_removed_tag_listener(boost::bind(&message_service::removed_tag_event, this, _1, _2, _3));
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
  tag_handle tag = nameserver_.register_tag(nameserver_.root(), topic);
  dispatcher_group<T>* group = get_dispatcher_group<T>(tag);
  local_dispatcher<T>* dispatcher = group->get_dispatcher(tag);
  tag->connect_new_tag_listener(boost::bind(&message_service::new_tag_event, this, _1, _2, _3));
  tag->connect_removed_tag_listener(boost::bind(&message_service::removed_tag_event, this, _1, _2, _3));
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
dispatcher_group<T>* message_service::get_dispatcher_group(const tag_handle& tag)
{
  typename dispatcher_group_list_type::iterator elem = dispatcher_group_list_.find(tag->id());
  if(elem == dispatcher_group_list_.end())
  {
    boost::shared_ptr<dispatcher_group<T> > group
      = boost::make_shared<dispatcher_group<T> >(

        &remote_dispatcher_);

    dispatcher_group_list_.insert(
      typename dispatcher_group_list_type::value_type(tag->id(), group));

    return group.get();
  }
  else
  {
    boost::shared_ptr<basic_dispatcher_group> &basic_dispatcher_group = elem->second;
    // todo, try
    boost::shared_ptr<dispatcher_group<T> > group
      = boost::dynamic_pointer_cast<dispatcher_group<T> >(basic_dispatcher_group);
    return group.get();
  }
}

}
}
