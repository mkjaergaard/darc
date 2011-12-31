/*
 * Copyright (c) 2011, Prevas A/S
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Prevas A/S nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * DARC Timer class
 *
 * \author Morten Kjaergaard
 */

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
