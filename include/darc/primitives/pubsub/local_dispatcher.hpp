#pragma once

#include <boost/asio.hpp>
#include <darc/primitives/pubsub/subscriber.hpp>
#include <darc/primitives/pubsub/publisher.hpp>
#include <darc/ns/tag_handle.hpp>
#include <darc/primitives/pubsub/message_service__decl.hpp>

namespace darc
{
namespace pubsub
{

template<typename T>
class dispatcher_group;

class local_dispatcher_base
{
public:
  virtual ~local_dispatcher_base()
  {
  }

  virtual void remote_message_recv(const ID& tag_id,
                                   darc::buffer::shared_buffer data) = 0;

  virtual void inform_remote_peer(const ID& peer_id) = 0;
};

template<typename T>
class local_dispatcher : public local_dispatcher_base
{
private:
  typedef std::vector<publisher_impl<T> *> publishers_list_type;
  typedef std::vector<subscriber_impl<T> *> subscribers_list_type;

  typedef serializer::boost_serializer serializer_type;

  publishers_list_type publishers_;
  subscribers_list_type subscribers_;

  message_service * message_service_;
  tag_handle tag_;

public:
  local_dispatcher(message_service * message_service, const tag_handle& tag) :
    message_service_(message_service),
    tag_(tag)
  {
  }

  void attach(subscriber_impl<T> &subscriber)
  {
    if(subscribers_.empty())
    {
      message_service_->send_subscription(ID::null(), tag_->id(),
                                                      tag_->name(),
                                                      "");
    }
    subscribers_.push_back(&subscriber);
  }

  void attach(publisher_impl<T> &publisher)
  {
    if(subscribers_.empty())
    {
      message_service_->send_publish(ID::null(), tag_->id(),
                                     tag_->name(),
                                     "");
    }
    publishers_.push_back(&publisher);
  }

  void detach(subscriber_impl<T> &subscriber)
  {
    for(typename subscribers_list_type::iterator it = subscribers_.begin();
        it != subscribers_.end();
        it++)
    {
      if(*it == &subscriber)
      {
        subscribers_.erase(it);
        return;
      }
    }
    assert(false); // detach of unknown subscriber
  }

  void detach(publisher_impl<T> &publisher)
  {
    for(typename publishers_list_type::iterator it = publishers_.begin();
        it != publishers_.end();
        it++)
    {
      if(*it == &publisher)
      {
        publishers_.erase(it);
        return;
      }
    }
    assert(false); // detach of unknown publisher
  }

  void dispatch_locally(const boost::shared_ptr<const T> &msg)
  {
    for(typename subscribers_list_type::iterator it = subscribers_.begin();
        it != subscribers_.end();
        it++)
    {
      (*it)->postCallback(msg);
    }
  }

  void dispatch_locally(const T &msg)
  {
    for(typename subscribers_list_type::iterator it = subscribers_.begin();
        it != subscribers_.end();
        it++)
    {
      (*it)->postCallback(msg);
    }
  }

  void dispatch_from_publisher(const boost::shared_ptr<const T> &msg)
  {
    dispatch_locally(msg);

    outbound_data<serializer_type, T> o_msg(*msg);
    message_service_->dispatch_remotely(tag_->id(), o_msg);
  }

  void dispatch_from_publisher(const T& msg)
  {
    dispatch_locally(msg);

    outbound_data<serializer_type, T> o_msg(msg);
    message_service_->dispatch_remotely(tag_->id(), o_msg);
  }

  void remote_message_recv(const ID& tag_id,
                           darc::buffer::shared_buffer data)
  {
    inbound_data_ptr<serializer_type, T> i_msg(data);
    dispatch_locally(i_msg.get());
  }

  void inform_remote_peer(const ID& peer_id)
  {
    if(!publishers_.empty())
    {
      message_service_->send_publish(ID::null(), tag_->id(),
                                     tag_->name(),
                                     "");
    }

    if(!subscribers_.empty())
    {
      message_service_->send_subscription(ID::null(), tag_->id(),
                                          tag_->name(),
                                          "");
    }
  }

};

}
}
