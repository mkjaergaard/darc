#pragma once

#include <darc/message_service__decl.hpp>

#include <boost/make_shared.hpp>
#include <darc/dispatcher_group.hpp>

#include <darc/id.hpp>

typedef darc::ID IDType;

namespace darc
{

template<typename T>
LocalDispatcher<T>* MessageService::attach(PublisherImpl<T> &publisher,
					   const std::string& topic)
{
  tag_handle tag = nameserver_.register_tag(nameserver_.root(), topic);
  DispatcherGroup<T>* group = getDispatcherGroup<T>(tag);
  LocalDispatcher<T>* dispatcher = group->getDispatcher(tag);
  dispatcher->attach(publisher);
  return dispatcher;
}

template<typename T>
LocalDispatcher<T>* MessageService::attach(SubscriberImpl<T> &subscriber,
					   const std::string& topic)
{
  tag_handle tag = nameserver_.register_tag(nameserver_.root(), topic);
  DispatcherGroup<T>* group = getDispatcherGroup<T>(tag);
  LocalDispatcher<T>* dispatcher = group->getDispatcher(tag);
  dispatcher->attach(subscriber);
  return dispatcher;
}

template<typename T>
DispatcherGroup<T>* MessageService::getDispatcherGroup(const tag_handle& tag)
{
  typename DispatcherGroupListType::iterator elem = dispatcher_group_list_.find(tag->id());
  if(elem == dispatcher_group_list_.end())
  {
    boost::shared_ptr<DispatcherGroup<T> > dispatcher_group
      = boost::make_shared<DispatcherGroup<T> >(
	boost::bind(&MessageService::new_tag_event, this, _1, _2, _3),
	&remote_dispatcher_);

    dispatcher_group_list_.insert(
      typename DispatcherGroupListType::value_type(tag->id(), dispatcher_group));

    return dispatcher_group.get();
  }
  else
  {
    boost::shared_ptr<BasicDispatcherGroup> &basic_dispatcher_group = elem->second;
    // todo, try
    boost::shared_ptr<DispatcherGroup<T> > dispatcher_group
      = boost::dynamic_pointer_cast<DispatcherGroup<T> >(basic_dispatcher_group);
    return dispatcher_group.get();
  }
}

}
