#include <darc/node.h>
#include <darc/component.h>
#include <darc/timer.h>
#include <darc/publish/subscriber.h>
#include <darc/procedure/client.h>
#include <std_msgs/String.h>

class Component2 : public darc::Component
{
public:
  darc::publish::Subscriber<std_msgs::String> sub_;
  darc::procedure::Client<std_msgs::String, std_msgs::String, std_msgs::String> procedure_client_;
  darc::Timer timer_;

  void subHandler( const boost::shared_ptr<std_msgs::String> msg )
  {
    std::cout << *msg << std::endl;
  }

  void timerHandler()
  {
    std::cout << "Timerevent C2" << std::endl;
    boost::shared_ptr<std_msgs::String> msg( new std_msgs::String() );
    msg->data = "AAAAAAARG";

    procedure_client_.call(msg);
  }

  void statusHandler( boost::shared_ptr<std_msgs::String> msg )
  {
    std::cout << "StatusHandler" << std::endl;
  }

  void returnHandler( boost::shared_ptr<std_msgs::String> msg )
  {
    std::cout << "ReturnHandler" << std::endl;
  }

  Component2( const std::string& instance_name, darc::Node::Ptr node ) :
    darc::Component(instance_name, node),
    sub_(this, "test", boost::bind(&Component2::subHandler, this, _1) ),
    procedure_client_( this, "some_proc", boost::bind( &Component2::returnHandler, this, _1 ),boost::bind( &Component2::statusHandler, this, _1 ) ),
    timer_(this, boost::bind(&Component2::timerHandler, this), boost::posix_time::seconds(10))
  {
  }

};

static int blah2 =  darc::ComponentRegister::registerComponent( "Component2", boost::bind(&darc::Component::instantiate<Component2>, _1, _2) );
