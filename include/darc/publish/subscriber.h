#ifndef __DARC_PUBLISH_SUBSCRIBER_H_INCLUDED__
#define __DARC_PUBLISH_SUBSCRIBER_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <darc/publish/subscriber_impl.h>
#include <darc/owner.h>

// Wraps a SubscriberImpl in a smart pointer so the lifetime of SubscriberImpl is dependent by the lifetime of Subscriber

namespace darc
{
namespace publish
{

template<typename T>
class Subscriber
{
private:
  boost::shared_ptr<SubscriberImpl<T> > impl_;

  typedef boost::function<void( boost::shared_ptr<T> )> CallbackType;

public:
  Subscriber(darc::Owner * owner, const std::string& topic, CallbackType callback) :
    impl_( new SubscriberImpl<T>( owner->getIOService(), topic, callback ) )
  {
    owner->getNode()->getPublisherManager().registerSubscriber(topic, impl_);
  }

  ~Subscriber()
  {
    // unregister subscriber
  } 

};

}
}

#endif
