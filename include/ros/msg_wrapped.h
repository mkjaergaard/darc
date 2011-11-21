#ifndef __ROS_EVENT_H_INCLUDED__
#define __ROS_EVENT_H_INCLUDED__

#include <boost/shared_ptr.hpp>

namespace ros
{

  class MsgWrappedAbstract
  {
  public:
    typedef boost::shared_ptr<MsgWrappedAbstract> Ptr;

  public:
    virtual ~MsgWrappedAbstract() {};
  };

  class MsgSerialized
  {
    
  }

  template<typename T>
  class MsgWrapped : public MsgWrappedAbstract
  {
  public:
    typedef boost::shared_ptr<MsgWrapped<T> > Ptr;

  public:
    MsgWrapped(boost::shared_ptr<T> &msg) :
      msg_(msg)
    {
    }

  public:
    boost::shared_ptr<T> msg_;
  };

}

#endif
