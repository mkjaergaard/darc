#ifndef __DARC_PUBLISHER_H_INCLUDED__
#define __DARC_PUBLISHER_H_INCLUDED__

#include <boost/smart_ptr.hpp>
#include <darc/node.h>
#include <darc/component.h>
#include <darc/publisher_impl.h>

namespace darc
{

template<typename T>
class Publisher
{
protected:
  boost::shared_ptr<PublisherImpl<T> > impl_;

public:
  Publisher(darc::Component* owner, const std::string& topic) :
  impl_( new PublisherImpl<T> )
  {
    owner->getNode()->registerPublisher<T>(topic, impl_);
  }

  void publish(boost::shared_ptr<T> msg)
  {
    impl_->publish(msg);
  }

};

}

#endif
