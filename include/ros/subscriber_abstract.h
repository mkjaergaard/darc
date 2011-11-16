#ifndef __ROS_SUBSCRIBER_ABSTRACT_H_INCLUDED__
#define __ROS_SUBSCRIBER_ABSTRACT_H_INCLUDED__

#include <ros/msg_wrapped.h>
#include <iostream>

namespace ros
{

class SubscriberAbstract
{
 public:
  virtual void Dispatch(MsgWrappedAbstract::Ptr msg) = 0;
};

}

#endif
