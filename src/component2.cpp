#include <darc/node.h>
#include <darc/component.h>
#include <darc/publisher.h>
#include <darc/subscriber.h>
#include <darc/timer.h>

class TestComponent2 : public darc::Component
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


  TestComponent2( darc::Node::Ptr node ) : darc::Component("component2", node),
					  sub_(this, "test", boost::bind(&TestComponent2::SubHandler, this, _1) ),
					  timer_(this, boost::bind(&TestComponent2::TimerHandler, this), boost::posix_time::seconds(10))
  {
  }

};

// wrap in macro if this is the way to do it
static int blah2 = darc::Node::RegisterComponent( new TestComponent2( darc::Node::Instance()) );

