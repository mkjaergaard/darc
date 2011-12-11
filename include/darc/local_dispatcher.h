#ifndef __DARC_LOCAL_DISPATCHER_H_INCLUDED___
#define __DARC_LOCAL_DISPATCHER_H_INCLUDED___

#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <darc/local_dispatcher_abstract.h>
#include <darc/subscriber_impl.h>
#include <darc/serialized_message.h>
#include <darc/remote_dispatch_handler.h>

namespace darc
{

template<typename T>
class LocalDispatcher : public LocalDispatcherAbstract
{
private:
  std::string topic_;
  RemoteDispatchHandler * remote_dispatch_handler_;

  typedef std::vector< boost::shared_ptr<SubscriberImpl<T> > > SubscriberListType; // <-- weak_ptr
  SubscriberListType subscriber_list_;

public:
  LocalDispatcher( const std::string& topic, RemoteDispatchHandler * remote_dispatch_handler ) :
    topic_(topic),
    remote_dispatch_handler_( remote_dispatch_handler ) {}

  void registerSubscriber( boost::shared_ptr<SubscriberImpl<T> > sub )
  {
    subscriber_list_.push_back( sub );
  }

  // Called by the local publishers  
  void dispatchMessage( boost::shared_ptr<T> msg )
  {
    dispatchMessageLocally(msg);
    remote_dispatch_handler_->postRemoteDispatch<T>(topic_, msg);
  }

  void dispatchMessageLocally( boost::shared_ptr<T> msg )
  {
    for( typename SubscriberListType::iterator it = subscriber_list_.begin();
         it != subscriber_list_.end();
         it++)
    {
      (*it)->dispatch( msg );
    }
  }

  // impl of virtual
  void dispatchMessageLocally( SerializedMessage::ConstPtr msg_s )
  {
    dispatchMessageLocally( msg_s->deserialize<T>() );
  }

};

}

#endif
