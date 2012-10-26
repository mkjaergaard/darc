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
public:
  typedef void(callback_type)(const boost::shared_ptr<const T>&);
  typedef boost::function<callback_type> callback_functor_type;

private:
  boost::asio::io_service &io_service_;
  MessageService &message_service_;

  LocalDispatcher<T> * dispatcher_; // ptr type?

  boost::signal<callback_type> callback_;

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

  void addCallback(callback_functor_type handler)
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
public:
  typedef typename SubscriberImpl<T>::callback_functor_type callback_functor_type;

private:
  boost::shared_ptr<SubscriberImpl<T> > impl_;

public:
  Subscriber()
  {
  }

  Subscriber(boost::asio::io_service &io_service,
	     MessageService &message_service) :
    impl_(boost::make_shared<SubscriberImpl<T> >(boost::ref(io_service),
						 boost::ref(message_service)))
  {
  }

  void attach(const std::string& topic)
  {
    if(impl_.get() != 0)
    {
      impl_->attach(topic);
    }
  }

  void detach()
  {
    if(impl_.get() != 0)
    {
      impl_->detach();
    }
  }

  void addCallback(boost::function<void(const boost::shared_ptr<const T>&)> handler)
  {
    if(impl_.get() != 0)
    {
      impl_->addCallback(handler);
    }
  }

};

}
