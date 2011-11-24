#include <darc/node.h>
#include <darc/component.h>
#include <darc/publisher.h>
#include <darc/timer.h>

class Component1 : public darc::Component
{
public:
  darc::Publisher<int> pub_;
  darc::Timer timer_;

  void TimerHandler( )
  {
    std::cout << "Timer" << std::endl;
    boost::shared_ptr<int> msg( new int(5) );
    pub_.Publish(msg);
  }
  
  Component1( const std::string& instance_name, darc::Node::Ptr node ) : darc::Component(instance_name, node),
		    pub_(this, "test"),
		    timer_(this, boost::bind(&Component1::TimerHandler, this), boost::posix_time::seconds(2))
  {
  }

};

static int blah =  darc::ComponentRegister::RegisterComponent( "Component1", boost::bind(&darc::Component::instantiate<Component1>, _1, _2) );
