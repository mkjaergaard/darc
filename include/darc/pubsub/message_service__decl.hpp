#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <darc/pubsub/basic_local_dispatcher__fwd.hpp>
#include <boost/bind.hpp>

#include <darc/id.hpp>
#include <darc/ns_service.hpp>
#include <darc/peer_service.hpp>
#include <darc/pubsub/remote_dispatcher.hpp>

namespace darc
{
namespace pubsub
{

// fwd
template<typename T>
class publisher_impl;

template<typename T>
class subscriber_impl;

template<typename T>
class dispatcher_group;
class basic_dispatcher_group;

template<typename T>
class local_dispatcher;

class message_service : public boost::noncopyable, public darc::peer_service
{
private:
  typedef std::map<darc::ID, boost::shared_ptr<basic_dispatcher_group> > dispatcher_group_list_type;
  dispatcher_group_list_type dispatcher_group_list_;

  boost::asio::io_service& io_service_;
  remote_dispatcher remote_dispatcher_;

public:
  ns_service& nameserver_;

  boost::mutex mutex_;

public:
  message_service(peer& p, boost::asio::io_service& io_service, ns_service& ns_service);

  // /////////////////////////////////////////////
  // Remap peer_service calls to remote_dispatcher
  //
  void recv(const darc::ID& src_peer_id,
            darc::buffer::shared_buffer data);

  // /////////////////////////////////////////////

  void new_tag_event(ID tag_id,
                     ID alias_id,
                     ID peer_id);

  void removed_tag_event(ID tag_id,
                         ID alias_id,
                         ID peer_id);

  void remote_message_recv(const ID& tag_id,
                           darc::buffer::shared_buffer data);

  template<typename T>
  local_dispatcher<T>* attach(publisher_impl<T> &publisher, const std::string &topic);

  template<typename T>
  local_dispatcher<T>* attach(subscriber_impl<T> &subscriber, const std::string &topic);

  template<typename T>
  void detach(publisher_impl<T> &publisher, local_dispatcher<T>* dispatcher);

  template<typename T>
  void detach(subscriber_impl<T> &subscriber, local_dispatcher<T>* dispatcher);

  template<typename T>
  dispatcher_group<T>* get_dispatcher_group(const std::string& topic);

  template<typename T>
  dispatcher_group<T>* get_dispatcher_group(const tag_handle& tag);

};

}
}
