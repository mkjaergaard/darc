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
  boost::posix_time::ptime expected_deadline_;

public:
  Timer(darc::Component * owner, CallbackType callback, boost::posix_time::time_duration period) :
    boost::asio::deadline_timer( *(owner->getIOService()), period ),
    callback_(callback),
    period_(period)
  {
    expected_deadline_ = boost::posix_time::microsec_clock::universal_time() + period;
    async_wait( boost::bind( &Timer::handler, this ) );
  }

  void handler()// const boost::system::error_code& error )
  {
    boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::universal_time() - expected_deadline_;
    expected_deadline_ += period_;
    //    std::cout << diff.total_milliseconds() << std::endl;
    expires_from_now( period_ - diff );

    async_wait( boost::bind( &Timer::handler, this ) );
    callback_();
  }

};

}

#endif
