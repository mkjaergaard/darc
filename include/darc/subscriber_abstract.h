#ifndef __DARC_SUBSCRIBER_ABSTRACT_H_INCLUDED__
#define __DARC_SUBSCRIBER_ABSTRACT_H_INCLUDED__

#include <iostream>
#include <darc/msg_wrapped.h>

namespace darc
{

class SubscriberAbstract
{
 public:
  virtual void Dispatch(MsgWrappedAbstract::Ptr msg) = 0;
};

}

#endif
