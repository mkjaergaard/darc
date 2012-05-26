#pragma once

#include <boost/asio.hpp>
#include <asmsg/subscriber.hpp>
#include <asmsg/publisher.hpp>

namespace asmsg
{
  
class BasicLocalDispatcher
{
public:
  virtual ~BasicLocalDispatcher()
  {
  }

};

template<typename T, class IDType>
class LocalDispatcher : public BasicLocalDispatcher
{
private:
  typedef std::vector<PublisherImpl<T, IDType> *> PublishersListType;
  typedef std::vector<SubscriberImpl<T, IDType> *> SubscribersListType;
  
  PublishersListType publishers_;
  SubscribersListType subscribers_;

public:
  void attach(SubscriberImpl<T, IDType> &subscriber)
  {
    std::cout << "sub" << std::endl;
    subscribers_.push_back(&subscriber);
  }

  void attach(PublisherImpl<T, IDType> &publisher)
  {
    std::cout << "pub" << std::endl;
    publishers_.push_back(&publisher);
  }

  void dispatchLocally(const boost::shared_ptr<const T> &msg)
  {
    std::cout << "out" << std::endl;
    for(typename SubscribersListType::iterator it = subscribers_.begin();
	it != subscribers_.end();
	it++)
      {
	(*it)->postCallback(msg);
	std::cout << "out2" << std::endl;
      }
  }

  void dispatchFromPublisher(const boost::shared_ptr<const T> &msg)
  {
    dispatchLocally(msg);
  }

};

}
