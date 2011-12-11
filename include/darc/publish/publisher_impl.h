#ifndef __DARC_PUBLISH_PUBLISHER_IMPL_H_INCLUDED__
#define __DARC_PUBLISH_PUBLISHER_IMPL_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <darc/publish/local_dispatcher.h>

namespace darc
{
namespace publish
{

template<typename T>
class PublisherImpl
{
protected:
  boost::weak_ptr<LocalDispatcher<T> > dispatcher_;

public:
  PublisherImpl()
  {
  }

  void registerDispatcher( boost::weak_ptr<LocalDispatcher<T> > dispatcher )
  {
    dispatcher_ = dispatcher;
  }

  void publish(boost::shared_ptr<T> msg)
  {
    if(boost::shared_ptr<LocalDispatcher<T> > dispatcher_sp = dispatcher_.lock())
    {
      dispatcher_sp->dispatchMessage(msg);
    }
  }

};

}
}

#endif
