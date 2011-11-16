#ifndef __ROS_NODE_H_INCLUDED__
#define __ROS_NODE_H_INCLUDED__

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <iostream>

#include <ros/component.h>

namespace ros
{

class Node
{
  typedef std::vector<ros::Component*> ComponentListType;
  ComponentListType component_list_;

  typedef std::map<const std::string, SubscriberAbstract* > SubscriberListType; // todo use weak ptr since we dont own it
  SubscriberListType subscriber_list_;


public:
  typedef boost::shared_ptr<Node> Ptr;
  
public:
  static boost::shared_ptr<Node> instance_;

  std::vector<boost::shared_ptr<boost::thread> > threads_;

public:
  static boost::shared_ptr<Node> Instance()
  {
    if( instance_.get() == 0 )
    {
      instance_.reset( new Node() );
    }
    return instance_;
  }

  static int RegisterComponent(ros::Component* component)
  {
    Instance()->component_list_.push_back(component);
    std::cout << "Registered Component: " << component->getName() << std::endl;

    boost::shared_ptr<boost::thread> thread(new boost::thread( boost::bind(&ros::Component::run, component)));
    Instance()->threads_.push_back(thread);

    return 0;
  }

  // called by the Subscriber
  void RegisterSubscriber( const std::string& topic, SubscriberAbstract* sub )
  {
    // todo: map cannot handle several subscribers per topic
    subscriber_list_[topic] = sub;
  }

  void DispatchToLocalSubscribers( const std::string& topic, boost::shared_ptr< MsgAbstract > msg )
  {
    if( subscriber_list_.count(topic) != 0 )
    {
      subscriber_list_[topic]->Dispatch(msg);
    }
  }

  template<typename T>
  void Publish(const std::string& topic, boost::shared_ptr<T> msg)
  {
    boost::shared_ptr< MsgImpl<T> > msg_impl( new MsgImpl<T>( msg ) );
    DispatchToLocalSubscribers( topic, msg_impl );
  }

};

}

#endif
