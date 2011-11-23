#include <darc/node.h>
#include <darc/component.h>
#include <darc/publisher.h>
#include <darc/timer.h>

class TestComponent : public darc::Component
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
  
  TestComponent( darc::Node::Ptr node ) : darc::Component("component1", node),
		    pub_(this, "test"),
		    timer_(this, boost::bind(&TestComponent::TimerHandler, this), boost::posix_time::seconds(2))
  {
  }

};

