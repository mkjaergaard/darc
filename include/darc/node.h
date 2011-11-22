#ifndef __DARC_NODE_H_INCLUDED__
#define __DARC_NODE_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <darc/local_dispatcher.h>
#include <darc/component.h>
#include <darc/subscriber_impl.h>
#include <darc/publisher_impl.h>

namespace darc
{

class Node
{
  typedef std::vector<darc::Component*> ComponentListType;
  ComponentListType component_list_;

  typedef std::map<const std::string, boost::shared_ptr<LocalDispatcherAbstract> > LocalDispatcherListType;
  LocalDispatcherListType local_dispatcher_list_;

public:
  typedef boost::shared_ptr<Node> Ptr;
  
public:
  // threading stuff
  static boost::shared_ptr<Node> instance_;
  std::vector<boost::shared_ptr<boost::thread> > threads_;

public:
  static boost::shared_ptr<Node> Instance() //todo: not thread safe
  {
    if( instance_.get() == 0 )
    {
      instance_.reset( new Node() );
    }
    return instance_;
  }

  static int RegisterComponent(darc::Component* component) //todo: not thread safe
  {
    Instance()->component_list_.push_back(component);
    std::cout << "Registered Component: " << component->getName() << std::endl;

    boost::shared_ptr<boost::thread> thread(new boost::thread( boost::bind(&darc::Component::run, component)));
    Instance()->threads_.push_back(thread);

    return 0;
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
  template<typename T>
  void DispatchToLocalSubscribers( const std::string& topic, boost::shared_ptr<T> &msg )
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
