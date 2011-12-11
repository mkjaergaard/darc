#ifndef __DARC_PUBLISH_PUBLISHER_H_INCLUDED__
#define __DARC_PUBLISH_PUBLISHER_H_INCLUDED__

#include <boost/smart_ptr.hpp>
#include <darc/node.h>
#include <darc/component.h>
#include <darc/publish/publisher_impl.h>

namespace darc
{
namespace publish
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
    owner->getNode()->getPublisherManager().registerPublisher<T>(topic, impl_);
  }

  void publish(boost::shared_ptr<T> msg)
  {
    impl_->publish(msg);
  }

};

}
}

#endif
