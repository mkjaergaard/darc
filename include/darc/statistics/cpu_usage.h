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
 * DARC CPUUsage class
 *
 * \author Morten Kjaergaard
 */

#pragma once

// todo: This stuff only works on linux, just disable on windows
#include <sys/time.h>
#include <sys/resource.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace darc
{
namespace statistics
{

class CPUUsage
{
protected:
  boost::posix_time::time_duration user_time_;
  boost::posix_time::time_duration system_time_;
  boost::posix_time::ptime start_time_;
  boost::posix_time::time_duration wall_time_;
  rusage start_usage_;
  bool measuring_;

public:
  CPUUsage() :
    user_time_(boost::posix_time::seconds(0)),
    system_time_(boost::posix_time::seconds(0)),
    wall_time_(boost::posix_time::seconds(0)),
    measuring_(false)
  {
  }

  void start()
  {
    assert(measuring_ == false);
    measuring_ = true;
    getrusage(RUSAGE_THREAD, &start_usage_);
    start_time_ = boost::posix_time::microsec_clock::universal_time();
    user_time_ = boost::posix_time::seconds(0);
    system_time_ = boost::posix_time::seconds(0);
    wall_time_ = boost::posix_time::seconds(0);
  }

  void measure()
  {
    assert(measuring_ == true);
    rusage usage;
    getrusage(RUSAGE_THREAD, &usage);

    user_time_ = boost::posix_time::seconds(usage.ru_utime.tv_sec - start_usage_.ru_utime.tv_sec) +
      boost::posix_time::microseconds(usage.ru_utime.tv_usec - start_usage_.ru_utime.tv_usec);

    system_time_ = boost::posix_time::seconds(usage.ru_stime.tv_sec - start_usage_.ru_stime.tv_sec) +
      boost::posix_time::microseconds(usage.ru_stime.tv_usec - start_usage_.ru_stime.tv_usec);

    wall_time_ = boost::posix_time::microsec_clock::universal_time() - start_time_;
  }

  void stop()
  {
    measure();
    measuring_ = false;
  }

  void reset()
  {
    user_time_ = boost::posix_time::seconds(0);
    system_time_ = boost::posix_time::seconds(0);
    measuring_ = false;
  }

  boost::posix_time::time_duration getUserCPUTime()
  {
    return user_time_;
  }

  boost::posix_time::time_duration getSystemCPUTime()
  {
    return system_time_;
  }

  boost::posix_time::time_duration getWallTime()
  {
    return wall_time_;
  }

};

}
}
