#ifndef __DARC_TIMER_H_INCLUDED__
#define __DARC_TIMER_H_INCLUDED__

#include <boost/asio.hpp>
#include <boost/function.hpp>

#include <darc/component.h>

namespace darc
{

class Timer : public boost::asio::deadline_timer
{

  typedef boost::function<void()> CallbackType;
  CallbackType callback_;

  boost::posix_time::time_duration period_;

public:
  Timer(darc::Component * owner, CallbackType callback, boost::posix_time::time_duration period) :
    boost::asio::deadline_timer( owner->GetIOService(), period ),
    callback_(callback),
    period_(period)
  {
    async_wait( boost::bind( &Timer::Handler, this ) );
  }

  void Handler()// const boost::system::error_code& error )
  {
    expires_from_now( period_ );
    async_wait( boost::bind( &Timer::Handler, this ) );
    callback_();
  }

};

}

#endif
