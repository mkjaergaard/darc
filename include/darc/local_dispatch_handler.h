#ifndef __DARC_LOCAL_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_LOCAL_DISPATCH_HANDLER_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <darc/remote_node_callback.h>
#include <darc/local_dispatcher.h>
#include <darc/subscriber_impl.h>
#include <darc/publisher_impl.h>
#include <darc/remote_dispatch_handler.h>

#include <std_msgs/String.h>

namespace darc
{

class LocalDispatchHandler
{
  typedef std::map<const std::string, boost::shared_ptr<LocalDispatcherAbstract> > LocalDispatcherListType;
  LocalDispatcherListType local_dispatcher_list_;

  RemoteDispatchHandler * remote_dispatch_handler_;

public:
  typedef boost::shared_ptr<Node> Ptr;
  
public:
  // link stuff
  boost::asio::io_service * io_service_;

public:
  LocalDispatchHandler() :
    io_service_(&io_service)
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

  // Will be called to dispatch remote messages
  // impl of virtual
  void dispatchFromRemoteNode( int id_, darc::SerializedMessage::ConstPtr msg_s )
  {
    assert(0);
  }

private:
  template<typename T>
  boost::shared_ptr<LocalDispatcher<T> > GetLocalDispatcher( const std::string& topic )
  {
    // do single lookup with
    if( local_dispatcher_list_.count( topic ) == 0 )
    {
      boost::shared_ptr<LocalDispatcher<T> > disp( new LocalDispatcher<T>() );
      local_dispatcher_list_[ topic ] = disp;
      return disp;
    }
    else
    {
      boost::shared_ptr<LocalDispatcherAbstract> disp_a = local_dispatcher_list_[topic];
      // todo, try
      boost::shared_ptr<LocalDispatcher<T> > disp = boost::dynamic_pointer_cast<LocalDispatcher<T> >(disp_a);
      return disp;
    }
    
  }
  


};

}

#endif
