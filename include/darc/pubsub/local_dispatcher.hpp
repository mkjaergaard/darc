#pragma once

#include <boost/asio.hpp>
#include <darc/pubsub/subscriber.hpp>
#include <darc/pubsub/publisher.hpp>
#include <darc/tag_handle.hpp>

namespace darc
{
namespace pubsub
{

template<typename T>
class dispatcher_group;

template<typename T>
class local_dispatcher
{
private:
  typedef std::vector<publisher_impl<T> *> publishers_list_type;
  typedef std::vector<subscriber_impl<T> *> subscribers_list_type;

  publishers_list_type publishers_;
  subscribers_list_type subscribers_;

  dispatcher_group<T> * group_;

public: //todo:
  tag_handle tag_;

public:
  local_dispatcher(dispatcher_group<T> * group, const tag_handle& tag) :
    group_(group),
    tag_(tag)
  {
  }

  void set_group(dispatcher_group<T> * group)
  {
    group_ = group;
  }

  void check_empty()
  {
    if(publishers_.empty() && subscribers_.empty())
    {
      group_->remove_dispatcher(this);
    }
  }

  void attach(subscriber_impl<T> &subscriber)
  {
    subscribers_.push_back(&subscriber);
  }

  void attach(publisher_impl<T> &publisher)
  {
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
        check_empty();
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
        check_empty();
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

  void dispatch_from_publisher(const boost::shared_ptr<const T> &msg)
  {
    group_->dispatch_to_group(tag_->id(), msg);
  }

};

}
}
