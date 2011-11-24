#include <darc/node.h>
#include <darc/component.h>
#include <darc/publisher.h>
#include <darc/subscriber.h>
#include <darc/timer.h>

class Component2 : public darc::Component
{
public:
  darc::Subscriber<int> sub_;
  darc::Timer timer_;

  void SubHandler( const boost::shared_ptr<int> msg )
  {
    std::cout << *msg << std::endl;
  }

  void TimerHandler()
  {
    std::cout << "Timerevent C2" << std::endl;
  }


  Component2( const std::string& instance_name, darc::Node::Ptr node ) : darc::Component(instance_name, node),
					  sub_(this, "test", boost::bind(&Component2::SubHandler, this, _1) ),
					  timer_(this, boost::bind(&Component2::TimerHandler, this), boost::posix_time::seconds(10))
  {
  }

};

static int blah2 =  darc::ComponentRegister::RegisterComponent( "Component2", boost::bind(&darc::Component::instantiate<Component2>, _1, _2) );
