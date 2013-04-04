#pragma once

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <darc/pubsub/message_service__decl.hpp>

#include <darc/id.hpp>

namespace darc
{
namespace pubsub
{

template<typename T>
class subscriber_impl
{
public:
  typedef void(callback_type)(const T&);
  typedef boost::function<callback_type> callback_functor_type;

private:
  boost::asio::io_service &io_service_;
  message_service &message_service_;

  local_dispatcher<T> * dispatcher_; // ptr type?

//  boost::signal<callback_type> callback_;
  callback_functor_type callback_;

public:
  subscriber_impl(boost::asio::io_service &io_service,
                 message_service &message_service) :
    io_service_(io_service),
    message_service_(message_service),
    dispatcher_(0)
  {
  }

  void attach(const std::string& topic)
  {
    dispatcher_ = message_service_.attach(*this, topic);
  }

  void detach()
  {
    if(dispatcher_ != 0)
    {
      message_service_.detach(*this, dispatcher_);
      dispatcher_ = 0;
    }
  }

  void addCallback(callback_functor_type handler)
  {
    //callback_.connect(handler);
    callback_ = handler;
  }

  void postCallback(const boost::shared_ptr<const T> &msg)
  {
    io_service_.post(boost::bind(&subscriber_impl::triggerCallback_, this, msg));
//    io_service_.post(callback_, msg);
  }

  void postCallback(const T &msg)
  {
    io_service_.post(boost::bind(&subscriber_impl::triggerCallback, this, msg));
//    io_service_.post(callback_, msg);
  }

  void triggerCallback_(const boost::shared_ptr<const T> &msg)
  {
    callback_(*msg);
  }

  void triggerCallback(const T &msg)
  {
    callback_(msg);
  }

};

template<typename T>
class subscriber
{
public:
  typedef typename subscriber_impl<T>::callback_functor_type callback_functor_type;

private:
  boost::shared_ptr<subscriber_impl<T> > impl_;

public:
  subscriber()
  {
  }

  subscriber(boost::asio::io_service &io_service,
             message_service &message_service) :
    impl_(boost::make_shared<subscriber_impl<T> >(boost::ref(io_service),
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

  void addCallback(callback_functor_type handler)
  {
    if(impl_.get() != 0)
    {
      impl_->addCallback(handler);
    }
  }

};

}
}
