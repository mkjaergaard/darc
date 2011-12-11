#ifndef __DARC_PUBLISHER_SUBSCRIBER_IMPL_H_INCLUDED__
#define __DARC_PUBLISHER_SUBSCRIBER_IMPL_H_INCLUDED__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace darc
{
namespace publish
{

template<typename T>
class SubscriberImpl
{
public:

private:
  typedef boost::shared_ptr<T> MsgPtrType;

  typedef boost::function<void(MsgPtrType)> CallbackType;
  CallbackType callback_;

  boost::asio::io_service * io_service_;

public:
  SubscriberImpl(boost::asio::io_service * io_service, const std::string& topic, CallbackType callback) :
    callback_(callback),
    io_service_(io_service)
  {
  }

  void dispatch( MsgPtrType &msg)
  {
    //typename MsgWrapped<T>::Ptr msg_wt = boost::dynamic_pointer_cast<MsgWrapped<T> >( msg_w );
    io_service_->post( boost::bind( &SubscriberImpl::receive, this, msg ) );
  }

private:
  void receive(MsgPtrType& msg)
  {
    callback_( msg );
  }

};

}
}

#endif
