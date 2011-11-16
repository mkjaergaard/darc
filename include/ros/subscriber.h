#ifndef __ROS_SUBSCRIBER_H_INCLUDED__
#define __ROS_SUBSCRIBER_H_INCLUDED__

#include <ros/event.h>
#include <boost/function.hpp>
#include <ros/component.h>
#include <ros/subscriber_abstract.h>
#include <iostream>

namespace ros
{

template<typename T>
class Subscriber : public SubscriberAbstract
{
private:
  typedef boost::shared_ptr<T> MsgPtrType;

  typedef boost::function<void(MsgPtrType)> CallbackType;
  CallbackType callback_;

  Component * owner_;

public:
  Subscriber(ros::Component * owner, const std::string& topic, CallbackType callback) :
    callback_(callback),
    owner_(owner)
  {
    owner->GetNode()->RegisterSubscriber(topic, this);
  }

  // impl
  void Dispatch(boost::shared_ptr<MsgAbstract> msg)
  {
    //todo: other ways than dynamic cast?
    boost::shared_ptr<MsgImpl<T> > msg_impl = boost::dynamic_pointer_cast<MsgImpl<T> >( msg );
    owner_->GetIOService().post( boost::bind( &Subscriber::Receive, this, msg_impl->msg_ ) );
    //    callback_( msg_impl->msg_ );
  }

  void Receive(MsgPtrType& msg)
  {
    callback_( msg );
  }

};

}

#endif
