#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <asmsg/basic_local_dispatcher__fwd.hpp>
#include <asmsg/local_dispatcher__fwd.hpp>

namespace asmsg
{

// fwd
template<typename T, class IDType>
class PublisherImpl;

template<typename T, class IDType>
class SubscriberImpl;

template<class IDType>
class MessageService : public boost::noncopyable
{
private:
  typedef std::map<IDType, boost::shared_ptr<BasicLocalDispatcher> > DispatcherListType; 
  DispatcherListType dispatcher_list_;

public:
  template<typename T>
  LocalDispatcher<T, IDType>* attach(PublisherImpl<T, IDType> &publisher, const IDType &topic_id);

  template<typename T>
  LocalDispatcher<T, IDType>* attach(SubscriberImpl<T, IDType> &subscriber, const IDType &topic_id);

  template<typename T>
  LocalDispatcher<T, IDType>* getDispatcher(const IDType& topic_id);

};

}
