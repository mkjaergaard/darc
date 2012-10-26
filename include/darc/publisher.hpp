#pragma once

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/signals.hpp>

#include <darc/message_service__decl.hpp>

#include <darc/id.hpp>

#include <boost/make_shared.hpp>

namespace darc
{

template<typename T>
class PublisherImpl
{
private:
  boost::asio::io_service &io_service_;
  MessageService &message_service_;

  LocalDispatcher<T> * dispatcher_; // ptr type?

public:
  PublisherImpl(boost::asio::io_service &io_service,
		MessageService &message_service) :
    io_service_(io_service),
    message_service_(message_service)
  {
  }

  void attach(const std::string& topic)
  {
    dispatcher_ = message_service_.template attach<T>(*this, topic);
  }

  void detach()
  {
    message_service_.detach(*this);
    dispatcher_ = 0;
  }

  void publish(const boost::shared_ptr<const T> &msg)
  {
    if(dispatcher_ != 0)
    {
      dispatcher_->dispatchFromPublisher(msg);
    }
  }

};

template<typename T>
class Publisher
{
private:
  boost::shared_ptr<PublisherImpl<T> > impl_;

public:
  Publisher()
  {
  }

  Publisher(boost::asio::io_service &io_service,
	    MessageService &message_service) :
    impl_(boost::make_shared<PublisherImpl<T> >(boost::ref(io_service),
						boost::ref(message_service)))
  {
  }

  void publish(const boost::shared_ptr<const T> &msg)
  {
    if(impl_.get() != 0)
    {
      impl_->publish(msg);
    }
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

};

}
