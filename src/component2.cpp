#include <ros/node.h>
#include <ros/component.h>
#include <ros/publisher.h>
#include <ros/subscriber.h>
#include <ros/timer.h>

class TestComponent2 : public ros::Component
{
public:
  ros::Subscriber<int> sub_;
  ros::Timer timer_;

  void SubHandler( const boost::shared_ptr<int> msg )
  {
    std::cout << *msg << std::endl;
  }

  void TimerHandler()
  {
    std::cout << "Timerevent C2" << std::endl;
  }


  TestComponent2( ros::Node::Ptr node ) : ros::Component("component2", node),
					  sub_(this, "test", boost::bind(&TestComponent2::SubHandler, this, _1) ),
					  timer_(this, boost::bind(&TestComponent2::TimerHandler, this), boost::posix_time::seconds(10))
  {
  }

};

// wrap in macro if this is the way to do it
static int blah2 = ros::Node::RegisterComponent( new TestComponent2( ros::Node::Instance()) );

