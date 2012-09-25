#pragma once

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <darc/message_service__decl.hpp>

#include <darc/id.hpp>

namespace darc
{

template<typename T>
class SubscriberImpl
{
private:
  boost::asio::io_service &io_service_;
  MessageService &message_service_;

  LocalDispatcher<T> * dispatcher_; // ptr type?

  boost::signal<void(const boost::shared_ptr<const T>&)> callback_;

public:
  SubscriberImpl(boost::asio::io_service &io_service,
		 MessageService &message_service) :
    io_service_(io_service),
    message_service_(message_service)
  {
  }

  void attach(const std::string& topic)
  {
    dispatcher_ = message_service_.attach(*this, topic);
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

template<typename T>
class Subscriber
{
private:
  boost::scoped_ptr<SubscriberImpl<T> > impl_;

public:
  Subscriber(boost::asio::io_service &io_service,
	     MessageService &message_service) :
    impl_(new SubscriberImpl<T>(io_service,
					message_service))
  {
  }

  void attach(const std::string& topic)
  {
    impl_->attach(topic);
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
