#include <darc/component.h>
#include <darc/pubsub/subscriber.h>

#include <std_msgs/String.h>

class MySubscriberComponent : public darc::Component
{
protected:
  darc::pubsub::Subscriber<std_msgs::String> sub_;

protected:
  void subHandler( const boost::shared_ptr<const std_msgs::String> msg )
  {
    std::cout << name_ << " received message: " << msg->data << std::endl;
  }

public:
  MySubscriberComponent( const std::string& instance_name, darc::Node::Ptr node ) :
    darc::Component(instance_name, node),
    sub_(this, "mytopic", boost::bind(&MySubscriberComponent::subHandler, this, _1) )
  {
  }

};

DARC_REGISTER_COMPONENT(MySubscriberComponent)
