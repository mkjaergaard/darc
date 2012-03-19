/*
 * Copyright (c) 2012, Prevas A/S
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
 * DARC Deadline Timer
 *
 * \author Morten Kjaergaard
 */

#include <darc/timer/deadline_timer.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <darc/log.h>

namespace darc
{
namespace timer
{

DeadlineTimer::DeadlineTimer(darc::Owner * owner, CallbackType callback) :
  boost::asio::deadline_timer(*(owner->getIOService())),
  callback_(callback)
{
  owner->addPrimitive(this->getWeakPtr());
}

void DeadlineTimer::start(boost::posix_time::time_duration time)
{
  expires_from_now(time);
  async_wait( boost::bind(&DeadlineTimer::handler, this, boost::asio::placeholders::error) );
}

void DeadlineTimer::onStart()
{
  // restart here?
}

void DeadlineTimer::onStop()
{
  cancel();
}

void DeadlineTimer::handler( const boost::system::error_code& error )
{
  if( error == boost::asio::error::operation_aborted )
  {
    // Happens if we cancel it
  }
  else if(error)
  {
    DARC_WARNING("DeadlineTimer callback gave some error %u", error.value());
  }
  else if( state_ == STOPPED )
  {
    // Just ignore it
  }
  else
  {
    if( state_ == RUNNING)
    {
      callback_();
    }
  }
}

}
}
