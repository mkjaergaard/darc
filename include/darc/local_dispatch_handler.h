#ifndef __DARC_LOCAL_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_LOCAL_DISPATCH_HANDLER_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <darc/local_dispatcher.h>
#include <darc/subscriber_impl.h>
#include <darc/publisher_impl.h>
#include <darc/remote_dispatch_handler.h>

namespace darc
{

class LocalDispatchHandler
{
  typedef std::map<const std::string, boost::shared_ptr<LocalDispatcherAbstract> > LocalDispatcherListType;
  LocalDispatcherListType local_dispatcher_list_;
  
public:
  // link stuff
  boost::asio::io_service * io_service_;
  RemoteDispatchHandler * remote_dispatch_handler_;  // todo: use bind instead

public:
  LocalDispatchHandler( boost::asio::io_service * io_service, RemoteDispatchHandler * remote_dispatch_handler ) :
    io_service_(io_service),
    remote_dispatch_handler_(remote_dispatch_handler)
  {
  }

  // called by the Subscriber
  // todo: not thread safe
  template<typename T>
  void RegisterSubscriber( const std::string& topic, boost::shared_ptr<SubscriberImpl<T> > sub )
  {
    boost::shared_ptr<LocalDispatcher<T> > disp = GetLocalDispatcher<T>(topic);
    disp->RegisterSubscriber( sub );
  }

  // Called by Publisher
  // todo: not thread safe
  template<typename T>
  void RegisterPublisher( const std::string& topic, boost::shared_ptr<PublisherImpl<T> > pub )
  {
    boost::shared_ptr<LocalDispatcher<T> > disp = GetLocalDispatcher<T>(topic);
    pub->RegisterDispatcher(disp);
  }

  void receiveFromRemoteNode( const std::string& topic, SerializedMessage::ConstPtr msg_s )
  {
    LocalDispatcherListType::iterator elem = local_dispatcher_list_.find(topic);
    if( elem != local_dispatcher_list_.end() )
    {
      elem->second->dispatchMessageLocally( msg_s );
    }
  }

private:
  template<typename T>
  boost::shared_ptr<LocalDispatcher<T> > GetLocalDispatcher( const std::string& topic )
  {
    // do single lookup with
    LocalDispatcherListType::iterator elem = local_dispatcher_list_.find(topic);
    if( elem == local_dispatcher_list_.end() )
    {
      boost::shared_ptr<LocalDispatcher<T> > disp( new LocalDispatcher<T>( topic, remote_dispatch_handler_ ) );
      local_dispatcher_list_[ topic ] = disp;
      return disp;
    }
    else
    {
      boost::shared_ptr<LocalDispatcherAbstract> &disp_a = elem->second;
      // todo, try
      boost::shared_ptr<LocalDispatcher<T> > disp = boost::dynamic_pointer_cast<LocalDispatcher<T> >(disp_a);
      return disp;
    }
  }

};

}

#endif
