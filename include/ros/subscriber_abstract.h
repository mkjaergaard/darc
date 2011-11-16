#ifndef __ROS_SUBSCRIBER_ABSTRACT_H_INCLUDED__
#define __ROS_SUBSCRIBER_ABSTRACT_H_INCLUDED__

#include <ros/event.h>
#include <iostream>

namespace ros
{

class SubscriberAbstract
{
 public:
  virtual void Dispatch(boost::shared_ptr<MsgAbstract> msg) = 0;
};

}

#endif
