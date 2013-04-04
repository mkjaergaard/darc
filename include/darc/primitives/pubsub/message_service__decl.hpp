#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <darc/id.hpp>
#include <darc/ns/ns_service.hpp>
#include <darc/peer/peer_service.hpp>
#include <darc/network/inbound_data.hpp>
#include <darc/serializer/boost.hpp>
#include <darc/serializer/ros.hpp>

#include <darc/network/payload_header_packet.hpp>
#include <darc/primitives/pubsub/message_packet.hpp>
#include <darc/primitives/pubsub/subscribe_packet.hpp>
#include <darc/primitives/pubsub/publish_packet.hpp>
#include <darc/primitives/pubsub/subscribed_topic_record.hpp>

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

  // Topics we publish (TODO: tmp hack, not really required by the msg system, should be handled differently)
  typedef std::set<subscribed_topic_record> published_topics_list_type;
  published_topics_list_type published_topics_list_;

  // New Topics
  typedef void(topic_change_callback_type)(bool event, const ID& peer_id, const std::string& topic, const std::string& type);
public:
  typedef boost::function<topic_change_callback_type> topic_change_functor_type;
protected:
  typedef boost::signal<topic_change_callback_type> topic_change_signal_type;
  topic_change_signal_type topic_change_signal_;

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

  void send_subscription(const ID& peer_id, const ID& tag_id, const std::string& tag_name, const std::string& type_name);
  void send_publish(const ID& peer_id, const ID& tag_id, const std::string& tag_name, const std::string& type_name);

  void recv(const darc::ID& src_peer_id,
            darc::buffer::shared_buffer data);

  void send_msg(const ID& tag_id, const ID& peer_id, const outbound_data_base& msg_data);

  void dispatch_remotely(const ID& tag_id, const outbound_data_base& msg_data);

  void handle_message_packet(const ID& remote_peer_id,
                             darc::buffer::shared_buffer data);
  void handle_publish_packet(const ID& remote_peer_id,
                             darc::buffer::shared_buffer data);
  void handle_subscribe_packet(const ID& remote_peer_id,
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

  ////////////////////////////
  // Topic Change Callback
  topic_change_signal_type& topic_change_signal()
  {
    return topic_change_signal_;
  }

};

}
}
