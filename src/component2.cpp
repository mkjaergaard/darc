#include <darc/node.h>
#include <darc/component.h>
#include <darc/timer.h>
#include <darc/publish/publisher.h>
#include <darc/publish/subscriber.h>
#include <std_msgs/String.h>

class Component2 : public darc::Component
{
public:
  darc::publish::Subscriber<std_msgs::String> sub_;
  darc::Timer timer_;

  void subHandler( const boost::shared_ptr<std_msgs::String> msg )
  {
    std::cout << *msg << std::endl;
  }

  void timerHandler()
  {
    std::cout << "Timerevent C2" << std::endl;
  }


  Component2( const std::string& instance_name, darc::Node::Ptr node ) : darc::Component(instance_name, node),
					  sub_(this, "test", boost::bind(&Component2::subHandler, this, _1) ),
					  timer_(this, boost::bind(&Component2::timerHandler, this), boost::posix_time::seconds(10))
  {
  }

};

static int blah2 =  darc::ComponentRegister::registerComponent( "Component2", boost::bind(&darc::Component::instantiate<Component2>, _1, _2) );
