#pragma once

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <asmsg/message_service__decl.hpp>

namespace asmsg
{

template<typename T, class IDType>
class SubscriberImpl
{
private:
  boost::asio::io_service &io_service_;
  MessageService<IDType> &message_service_;

  LocalDispatcher<T, IDType> * dispatcher_; // ptr type?

  boost::signal<void(const boost::shared_ptr<const T>&)> callback_;

public:
  SubscriberImpl(boost::asio::io_service &io_service,
		 MessageService<IDType> &message_service) :
    io_service_(io_service),
    message_service_(message_service)
  {
  }

  void attach(const IDType& topic_id)
  {
    dispatcher_ = message_service_.attach(*this, topic_id);
  }

  void detach()
  {
    message_service_.detach(*this);
    dispatcher_ = 0;
  }

  void addCallback(boost::function<void(const boost::shared_ptr<const T>&)> handler)
  {
    callback_.connect(handler);
  }

  void postCallback(const boost::shared_ptr<const T> &msg)
  {
    io_service_.post(boost::bind(&SubscriberImpl::triggerCallback, this, msg));
  }

  void triggerCallback(const boost::shared_ptr<const T> msg)
  {
    callback_(msg);
  }

};

template<typename T, class IDType>
class Subscriber
{
private:
  boost::scoped_ptr<SubscriberImpl<T, IDType> > impl_;

public:
  Subscriber(boost::asio::io_service &io_service,
	     MessageService<IDType> &message_service) :
    impl_(new SubscriberImpl<T, IDType>(io_service,
					message_service))
  {
  }

  void attach(const IDType& topic_id)
  {
    impl_->attach(topic_id);
  }

  void detach()
  {
    impl_->detach();
  }

  void addCallback(boost::function<void(const boost::shared_ptr<const T>&)> handler)
  {
    impl_->addCallback(handler);
  }

};

}
