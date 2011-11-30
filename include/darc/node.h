#ifndef __DARC_NODE_H_INCLUDED__
#define __DARC_NODE_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <darc/local_dispatcher.h>
#include <darc/subscriber_impl.h>
#include <darc/publisher_impl.h>
#include <darc/remote_dispatch_handler.h>
#include <darc/local_dispatch_handler.h>

#include <std_msgs/String.h>

namespace darc
{

class Node
{
  typedef std::map<const std::string, boost::shared_ptr<LocalDispatcherAbstract> > LocalDispatcherListType;
  LocalDispatcherListType local_dispatcher_list_;

public:
  typedef boost::shared_ptr<Node> Ptr;
  
private:
  boost::asio::io_service io_service_;

  RemoteDispatchHandler remote_dispatch_handler_;
  LocalDispatchHandler local_dispatch_handler_;

public:
  Node() :
    remote_dispatch_handler_(&io_service_),
    local_dispatch_handler_(&io_service_, &remote_dispatch_handler_)
  {
  }

  void doSomeFun()
  {
    /*
    boost::shared_ptr<boost::thread> thread(new boost::thread( boost::bind(&darc::Node::run, this)));
    Instance()->threads_.push_back(thread);

    udp1_.addRemoteNode( 0, "127.0.0.1", "19001");

    boost::shared_ptr<SerializedMessage> msg_s( new SerializedMessage() );
    boost::shared_ptr<std_msgs::String> b( new std_msgs::String() );
    b->data = "test";
    msg_s->serializeMsg(b);
    
    udp1_.dispatch( 0, msg_s);
    */
  }

  void run()
  {
    std::cout << "Running Node" << std::endl;
    io_service_.run();
  }

  // called by the Subscriber
  // todo: not thread safe
  template<typename T>
  void RegisterSubscriber( const std::string& topic, boost::shared_ptr<SubscriberImpl<T> > sub )
  {
    local_dispatch_handler_.RegisterSubscriber<T>(topic, sub);
    //    boost::shared_ptr<LocalDispatcher<T> > disp = GetLocalDispatcher<T>(topic);
    //disp->RegisterSubscriber( sub );
  }

  // Called by Publisher
  // todo: not thread safe
  template<typename T>
  void RegisterPublisher( const std::string& topic, boost::shared_ptr<PublisherImpl<T> > pub )
  {
    local_dispatch_handler_.RegisterPublisher<T>(topic, pub);
    //    boost::shared_ptr<LocalDispatcher<T> > disp = GetLocalDispatcher<T>(topic);
    //    pub->RegisterDispatcher(disp);
  }

  // Will be called to dispatch remote messages
  template<typename T>
  void DispatchToLocalSubscribers( const std::string& topic, boost::shared_ptr<T> &msg )
  {
    assert(0);
  }
  /*
private:
  template<typename T>
  boost::shared_ptr<LocalDispatcher<T> > GetLocalDispatcher( const std::string& topic )
  {
    // do single lookup with
    if( local_dispatcher_list_.count( topic ) == 0 )
    {
      boost::shared_ptr<LocalDispatcher<T> > disp( new LocalDispatcher<T>( &remote_dispatch_handler_) );
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
  */


};

}

#endif
