#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <darc/basic_local_dispatcher__fwd.hpp>
//#include <darc/dispatcher_group__fwd.hpp>

#include <darc/id.hpp>
#include <darc/ns_service.hpp>

#include <boost/bind.hpp>

namespace darc
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
  ns_service& nameserver_;

public:
  MessageService(boost::asio::io_service& io_service, ns_service& ns_service) :
    io_service_(io_service),
    nameserver_(ns_service)
  {
  }

  void post_new_tag_event(ID tag_id,
			  ID alias_id,
			  ID peer_id)
  {
    io_service_.post(boost::bind(&MessageService::new_tag_event, this, tag_id, alias_id, peer_id));
  }

  void new_tag_event(ID tag_id,
		     ID alias_id,
		     ID peer_id);

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
  DispatcherGroup<T>* getDispatcherGroup(const tag_handle& tag);

};

}
