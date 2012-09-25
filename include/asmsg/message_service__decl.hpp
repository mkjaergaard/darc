#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <asmsg/basic_local_dispatcher__fwd.hpp>
//#include <asmsg/dispatcher_group__fwd.hpp>

#include <darc/id.hpp>
#include <hns/server_entity.hpp>

#include <boost/bind.hpp>

namespace asmsg
{

// fwd
template<typename T>
class PublisherImpl;

template<typename T>
class SubscriberImpl;

template<typename T>
class DispatcherGroup;
class BasicDispatcherGroup;

template<typename T>
class LocalDispatcher;

class MessageService : public boost::noncopyable
{
private:
  typedef std::map<darc::ID, boost::shared_ptr<BasicDispatcherGroup> > DispatcherGroupListType;
  DispatcherGroupListType dispatcher_group_list_;

  boost::asio::io_service& io_service_;

public:
  hns::ServerEntity nameserver_;

public:
  MessageService(boost::asio::io_service& io_service) :
    io_service_(io_service)
  {
  }

  void tagEvent(ID tag_id,
		ID alias_id,
		hns::TagEvent event)
  {
    io_service_.post(boost::bind(&MessageService::doTagEvent, this, tag_id, alias_id, event));
  }

  void doTagEvent(ID tag_id,
		  ID alias_id,
		  hns::TagEvent event);

  template<typename T>
  LocalDispatcher<T>* attach(PublisherImpl<T> &publisher, const std::string &topic);

  template<typename T>
  LocalDispatcher<T>* attach(SubscriberImpl<T> &subscriber, const std::string &topic);

  template<typename T>
  void detach(SubscriberImpl<T> &subscriber)
  {
  }

  template<typename T>
  DispatcherGroup<T>* getDispatcherGroup(const std::string& topic);

  template<typename T>
  DispatcherGroup<T>* getDispatcherGroup(const hns::TagHandle& tag);

};

}
