#include <darc/node.h>
#include <darc/component.h>
#include <darc/publish/publisher.h>
#include <darc/timer.h>
#include <darc/procedure/server.h>

#include <std_msgs/String.h>

class Component1 : public darc::Component
{
public:
  darc::publish::Publisher<std_msgs::String> pub_;
  darc::procedure::Server<std_msgs::String, std_msgs::String, std_msgs::String> procedure_server_;
  darc::Timer timer_;

  void timerHandler( )
  {
    std::cout << "Timer" << std::endl;
    boost::shared_ptr<std_msgs::String> msg( new std_msgs::String() );
    msg->data = "blop";
    pub_.publish(msg);
  }
  
  void procedureCall( boost::shared_ptr<std_msgs::String> msg )
  {
    std::cout << "Procedure Call" << std::endl;
  }

  Component1( const std::string& instance_name, darc::Node::Ptr node ) : 
    darc::Component(instance_name, node),
    pub_(this, "test"),
    procedure_server_( this, "some_proc", boost::bind(&Component1::procedureCall, this, _1) ),
    timer_(this, boost::bind(&Component1::timerHandler, this), boost::posix_time::seconds(2))
  {
  }

};

static int blah =  darc::ComponentRegister::registerComponent( "Component1", boost::bind(&darc::Component::instantiate<Component1>, _1, _2) );
