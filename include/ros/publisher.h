#include <ros/node.h>
#include <ros/component.h>

namespace ros
{

template<typename T>
class Publisher
{
 protected:
  std::string topic_;
  ros::Component* owner_;

 public:
 Publisher(ros::Component* owner, const std::string& topic) :
  topic_(topic),
    owner_(owner)
  {
    // todo: register with Node to set up remote publishing
  }

  void Publish(boost::shared_ptr<T> msg)
  {
    // todo: best interface to Node not clear
    owner_->GetNode()->Publish(topic_, msg);
  }

};

}


