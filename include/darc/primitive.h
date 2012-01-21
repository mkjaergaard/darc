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
 * DARC Primitive class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <darc/enable_weak_from_static.h>
#include <darc/id.h>

namespace darc
{

class Primitive
{
protected:
  typedef enum {STOPPED, PAUSED, RUNNING} StateType;

  StateType state_;
  ID id_;

  virtual void onPause() {}
  virtual void onUnpause() {}
  virtual void onStop() {}
  virtual void onStart() {}

public:
  Primitive():
    state_(STOPPED),
    id_(createID())
  {}

  const ID& getID() const
  {
    return id_;
  }

  void pause()
  {
    if( state_ == RUNNING )
    {
      state_ = PAUSED;
      onPause();
    }
  }

  void unpause()
  {
    if( state_ == PAUSED )
    {
      state_ = RUNNING;
      onUnpause();
    }
  }

  void stop()
  {
    if( state_ != STOPPED )
    {
      state_ = STOPPED;
      onStop();
    }
  }

  void start()
  {
    if( state_ == STOPPED )
    {
      state_ = RUNNING;
      onStart();
    }
  }

};

}
