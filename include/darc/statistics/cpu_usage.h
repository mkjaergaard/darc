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

#include <sys/time.h>
#include <sys/resource.h>


namespace darc
{
namespace statistics
{

class CPUUsage
{
protected:
  int32_t usage_sec_;
  int32_t usage_usec_;

  int32_t start_sec_;
  int32_t start_usec_;
  bool measuring_;

public:
  CPUUsage() :
    usage_sec_(0),
    usage_usec_(0),
    start_sec_(0),
    start_usec_(0),
    measuring_(false)
  {
  }

  void start()
  {
    assert(measuring_ == false);
    measuring_ = true;

    rusage usage;
    getrusage(RUSAGE_THREAD, &usage);
    start_sec_ = usage.ru_utime.tv_sec;
    start_usec_ = usage.ru_utime.tv_usec;
  }

  void stop()
  {
    assert(measuring_ == true);
    measuring_ = false;

    rusage usage;
    getrusage(RUSAGE_THREAD, &usage);
    usage_sec_ += usage.ru_utime.tv_sec - start_sec_;
    int32_t usec = usage.ru_utime.tv_usec - start_usec_;
    if( usec > 1000000 )
    {
      usage_sec_++;
      usec -= 1000000;
    }
    usage_usec_ += usec;
  }

};

}
}
