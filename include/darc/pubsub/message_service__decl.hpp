#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <darc/id.hpp>
#include <darc/ns_service.hpp>
#include <darc/peer_service.hpp>
#include <darc/inbound_data.hpp>
#include <darc/serializer/boost.hpp>
#include <darc/serializer/ros.hpp>

#include <darc/payload_header_packet.hpp>
#include <darc/pubsub/message_packet.hpp>
#include <darc/pubsub/subscribe_packet.hpp>
#include <darc/pubsub/publish_packet.hpp>
#include <darc/pubsub/subscribed_topic_record.hpp>

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
class local_dispatcher;
class local_dispatcher_base;

class message_service : public boost::noncopyable, public darc::peer_service
{
private:
  typedef std::map<darc::ID, boost::shared_ptr<local_dispatcher_base> > dispatcher_list_type;
  dispatcher_list_type dispatcher_list_;

  boost::asio::io_service& io_service_;

  // Stuff for managing topics
  boost::signals::connection peer_connected_connection_;
  boost::signals::connection peer_disconnected_connection_;

  // Topics we subscribe to
  typedef std::list<subscribed_topic_record> subscribed_topics_list_type;
  subscribed_topics_list_type subscribed_topics_list_;

  // Topics we publish (todo: tmp hack, not really required by the msg system, should be handled differently)
  typedef std::set<subscribed_topic_record> published_topics_list_type;
  published_topics_list_type published_topics_list_;

public:
  ns_service& nameserver_;

  boost::mutex mutex_;

public:
  message_service(peer& p, boost::asio::io_service& io_service, ns_service& ns_service);

  // /////////////////////////////////////////////
  // Network Stuff
  //
  void peer_connected_handler(const ID& peer_id);
  void peer_disconnected_handler(const ID& peer_id);

  void send_subscription(const ID& tag_id, const ID& peer_id);
  void send_publish(const ID& tag_id, const ID& peer_id);

  void recv(const darc::ID& src_peer_id,
            darc::buffer::shared_buffer data);

  template<typename T>
  void send_msg(const ID& tag_id, const ID& peer_id, const boost::shared_ptr<const T> &msg);

  template<typename T>
  void dispatch_remotely(const ID& tag_id, const boost::shared_ptr<const T> &msg);

  void remote_message_recv(const ID& tag_id,
                           darc::buffer::shared_buffer data);
  ///////////////////////////
  // Local dispatchers stuff
  template<typename T>
  local_dispatcher<T>* attach(publisher_impl<T> &publisher, const std::string &topic);

  template<typename T>
  local_dispatcher<T>* attach(subscriber_impl<T> &subscriber, const std::string &topic);

  template<typename T>
  void detach(publisher_impl<T> &publisher, local_dispatcher<T>* dispatcher);

  template<typename T>
  void detach(subscriber_impl<T> &subscriber, local_dispatcher<T>* dispatcher);

  template<typename T>
  local_dispatcher<T>* get_dispatcher(const tag_handle& tag);

};

}
}
