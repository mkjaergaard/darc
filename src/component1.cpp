#include <ros/node.h>
#include <ros/component.h>
#include <ros/publisher.h>
#include <ros/timer.h>

class TestComponent : public ros::Component
{
public:
  ros::Publisher<int> pub_;
  ros::Timer timer_;

  void TimerHandler( )
  {
    std::cout << "Timer" << std::endl;
    boost::shared_ptr<int> msg( new int(5) );
    pub_.Publish(msg);
  }
  
  TestComponent( ros::Node::Ptr node ) : ros::Component("component1", node),
		    pub_(this, "test"),
		    timer_(this, boost::bind(&TestComponent::TimerHandler, this), boost::posix_time::seconds(2))
  {
  }

};

// wrap in macro if this is the way to do it
static int blah = ros::Node::RegisterComponent( new TestComponent( ros::Node::Instance()) );

