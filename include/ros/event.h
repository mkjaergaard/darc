#ifndef __ROS_EVENT_H_INCLUDED__
#define __ROS_EVENT_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <string>

namespace ros
{

  class MsgAbstract
  {
  public:
    virtual ~MsgAbstract() {};
  };

  template<typename T>
  class MsgImpl : public MsgAbstract
  {
  public:
    MsgImpl(boost::shared_ptr<T> &msg) :
      msg_(msg)
    {
    }

  public:
    boost::shared_ptr<T> msg_;
  };

  class Event
  {
  public:
    std::string topic_;
    MsgAbstract * msg_;
   
  public:
    Event(const std::string& topic, MsgAbstract * msg) :
      topic_(topic),
      msg_(msg)
    {
    }
    
  };

}

#endif
