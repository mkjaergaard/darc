#ifndef __DARC_SUBSCRIBER_H_INCLUDED__
#define __DARC_SUBSCRIBER_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <darc/subscriber_impl.h>

// Wraps a SubscriberImpl in a smart pointer so the lifetime of SubscriberImpl is dependent by the lifetime of Subscriber

namespace darc
{

template<typename T>
class Subscriber
{
private:
  boost::shared_ptr<SubscriberImpl<T> > impl_;

  typedef boost::function<void( boost::shared_ptr<T> )> CallbackType;

public:
  Subscriber(darc::Component * owner, const std::string& topic, CallbackType callback) :
  impl_( new SubscriberImpl<T>( owner->getIOService(), topic, callback ) )
  {
    owner->getNode()->registerSubscriber(topic, impl_);
  }

  ~Subscriber()
  {
    // unregister subscriber
  } 

};

}

#endif
