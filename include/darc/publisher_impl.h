#ifndef __DARC_PUBLISHER_IMPL_H_INCLUDED__
#define __DARC_PUBLISHER_IMPL_H_INCLUDED__

#include <darc/local_dispatcher.h>

#include <boost/shared_ptr.hpp>

namespace darc
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

  void RegisterDispatcher( boost::weak_ptr<LocalDispatcher<T> > dispatcher )
  {
    dispatcher_ = dispatcher;
  }

  void Publish(boost::shared_ptr<T> msg)
  {
    if(boost::shared_ptr<LocalDispatcher<T> > dispatcher_sp = dispatcher_.lock())
    {
      dispatcher_sp->DispatchMessage(msg);
    }
  }

};

}

#endif
