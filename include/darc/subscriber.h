#ifndef __DARC_SUBSCRIBER_H_INCLUDED__
#define __DARC_SUBSCRIBER_H_INCLUDED__

#include <iostream>
#include <boost/function.hpp>
#include <darc/msg_wrapped.h>
#include <darc/component.h>
#include <darc/subscriber_abstract.h>

namespace darc
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
  Subscriber(darc::Component * owner, const std::string& topic, CallbackType callback) :
    callback_(callback),
    owner_(owner)
  {
    owner->GetNode()->RegisterSubscriber(topic, this);
  }

  // impl
  void Dispatch(MsgWrappedAbstract::Ptr msg_w)
  {
    //todo: other ways than dynamic cast?
    typename MsgWrapped<T>::Ptr msg_wt = boost::dynamic_pointer_cast<MsgWrapped<T> >( msg_w );
    owner_->GetIOService().post( boost::bind( &Subscriber::Receive, this, msg_wt->msg_ ) );
  }

  void Receive(MsgPtrType& msg)
  {
    callback_( msg );
  }

};

}

#endif
