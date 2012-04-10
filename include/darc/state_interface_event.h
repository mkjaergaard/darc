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
 * DARC State Interface Event
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <boost/function.hpp>
#include <boost/signals.hpp>
#include <boost/bind.hpp>
#include <darc/owner.h>

namespace darc
{

template<typename T1, typename T2, typename T3, typename T4>
class StateInterfaceEvent4
{
public:
  typedef boost::function<void(const T1&, const T2&, const T3&, const T4&)> CallbackType;
  CallbackType callback_;

protected:
  boost::signals::connection connection_;
  Owner * owner_;

public:
  StateInterfaceEvent4(darc::Owner* owner) :
    owner_(owner)
  {
  }

  void connect(boost::signal<void(const T1&, const T2&, const T3&, const T4&)>& signal)
  {
    connection_ = signal.connect(boost::bind(&StateInterfaceEvent4::postEvent, this, _1, _2, _3, _4));
  }

  void addListener(CallbackType callback)
  {
    callback_ = callback;
  }

  void postEvent(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)
  {
    owner_->getIOService()->post(boost::bind(&StateInterfaceEvent4::triggerEvent, this, arg1, arg2, arg3, arg4));
  }

private:
  void triggerEvent(const T1 arg1, const T2 arg2, const T3 arg3, const T4 arg4)
  {
    callback_(arg1, arg2, arg3, arg4);
  }

};

}
