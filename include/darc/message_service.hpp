#pragma once

#include <darc/message_service__decl.hpp>

#include <boost/make_shared.hpp>
#include <darc/dispatcher_group.hpp>

#include <darc/id.hpp>

typedef darc::ID IDType;

namespace darc
{

void MessageService::doTagEvent(ID tag_id,
		ID alias_id,
		hns::TagEvent event)
{
  beam::glog<beam::Info>("tagEvent",
			 "tag_id", beam::arg<darc::ID>(tag_id),
			 "alias_id", beam::arg<darc::ID>(alias_id),
			 "event", beam::arg<int>(event));

  DispatcherGroupListType::iterator elem1 = dispatcher_group_list_.find(tag_id);
  DispatcherGroupListType::iterator elem2 = dispatcher_group_list_.find(alias_id);

  if(elem1 == dispatcher_group_list_.end() ||
     elem2 == dispatcher_group_list_.end())
  {
    beam::glog<beam::Warning>("tagEvent, but no dispatcher_group");
    return;
  }

  beam::glog<beam::Info>("pointers",
			 "1", beam::arg<void*>(elem1->second.get()),
			 "2", beam::arg<void*>(elem2->second.get()));

  if(elem1->second.get() != elem2->second.get())
  {
    elem1->second->join(elem2->second);
    elem2->second = elem1->second;
  }

}


template<typename T>
LocalDispatcher<T>* MessageService::attach(PublisherImpl<T> &publisher,
					   const std::string& topic)
{
  hns::TagHandle tag = nameserver_.registerTag(topic);
  DispatcherGroup<T>* group = getDispatcherGroup<T>(tag);
  LocalDispatcher<T>* dispatcher = group->getDispatcher(tag);
  dispatcher->attach(publisher);
  return dispatcher;
}

template<typename T>
LocalDispatcher<T>* MessageService::attach(SubscriberImpl<T> &subscriber,
					   const std::string& topic)
{
  hns::TagHandle tag = nameserver_.registerTag(topic);
  DispatcherGroup<T>* group = getDispatcherGroup<T>(tag);
  LocalDispatcher<T>* dispatcher = group->getDispatcher(tag);
  dispatcher->attach(subscriber);
  return dispatcher;
}

template<typename T>
DispatcherGroup<T>* MessageService::getDispatcherGroup(const hns::TagHandle& tag)
{
  typename DispatcherGroupListType::iterator elem = dispatcher_group_list_.find(tag.id());
  if(elem == dispatcher_group_list_.end())
  {
    boost::shared_ptr<DispatcherGroup<T> > dispatcher_group
      = boost::make_shared<DispatcherGroup<T> >(
	boost::bind(&MessageService::tagEvent, this, _1, _2, _3));

    dispatcher_group_list_.insert(
      typename DispatcherGroupListType::value_type(tag.id(), dispatcher_group));

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
