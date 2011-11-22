#ifndef __DARC_PUBLISHER_H_INCLUDED__
#define __DARC_PUBLISHER_H_INCLUDED__

#include <darc/node.h>
#include <darc/component.h>

#include <boost/smart_ptr.hpp>

#include <darc/publisher_impl.h>

namespace darc
{

template<typename T>
class Publisher
{
protected:
  darc::Component* owner_;
  boost::shared_ptr<PublisherImpl<T> > impl_;

public:
  Publisher(darc::Component* owner, const std::string& topic) :
  impl_( new PublisherImpl<T> )
  {
    owner->GetNode()->RegisterPublisher<T>(topic, impl_);
  }

  void Publish(boost::shared_ptr<T> msg)
  {
    impl_->Publish(msg);
  }

};

}

#endif
