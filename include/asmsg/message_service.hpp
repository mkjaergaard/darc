#pragma once

#include <asmsg/message_service__decl.hpp>

#include <boost/make_shared.hpp>
#include <asmsg/local_dispatcher.hpp>

#include <boost/uuid/uuid_io.hpp> //tmp

namespace asmsg
{

template<class IDType>
template<typename T>
LocalDispatcher<T, IDType>* MessageService<IDType>::attach(PublisherImpl<T, IDType> &publisher,
							   const IDType& topic_id)
{
  LocalDispatcher<T, IDType>* dispatcher = getDispatcher<T>(topic_id);
  dispatcher->attach(publisher);
  return dispatcher;
}

template<class IDType>
template<typename T>
LocalDispatcher<T, IDType>* MessageService<IDType>::attach(SubscriberImpl<T, IDType> &subscriber,
							   const IDType& topic_id)
{
  LocalDispatcher<T, IDType>* dispatcher = getDispatcher<T>(topic_id);
  dispatcher->attach(subscriber);
  return dispatcher;
}

template<class IDType>
template<typename T>
LocalDispatcher<T, IDType>* MessageService<IDType>::getDispatcher(const IDType& topic_id)
{
  std::cout << "cnt " << dispatcher_list_.count(topic_id);
  typename DispatcherListType::iterator elem = dispatcher_list_.find(topic_id);
  if(elem == dispatcher_list_.end())
  {
    std::cout << "new" << to_string(topic_id) << std::endl;

    boost::shared_ptr<LocalDispatcher<T, IDType> > dispatcher
      = boost::make_shared<LocalDispatcher<T, IDType> >();//topic_id, this);

    dispatcher_list_.insert(typename DispatcherListType::value_type(topic_id, dispatcher));
    std::cout << "cnt " << dispatcher_list_.count(topic_id);
    return dispatcher.get();
  }
  else
  {
    std::cout << "old" << std::endl;
    boost::shared_ptr<BasicLocalDispatcher> &basic_dispatcher = elem->second;
    // todo, try
    boost::shared_ptr<LocalDispatcher<T, IDType> > dispatcher
      = boost::dynamic_pointer_cast<LocalDispatcher<T, IDType> >(basic_dispatcher);
    return dispatcher.get();
  }
}

}
