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
 * DARC Server class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <darc/procedure/server_decl.h>
#include <darc/procedure/manager.h>
#include <darc/procedure/local_dispatcher.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Result, typename T_Feedback>
void Server<T_Arg, T_Result, T_Feedback>::onStart()
{
  boost::shared_ptr<LocalDispatcher<T_Arg, T_Result, T_Feedback> > dispatcher =
    owner_->getNode()->getProcedureManager().getLocalDispatcher<T_Arg, T_Result, T_Feedback>(name_);
  dispatcher->registerServer(this);
  dispatcher_ = dispatcher;
}

template<typename T_Arg, typename T_Result, typename T_Feedback>
void Server<T_Arg, T_Result, T_Feedback>::onStop()
{
  assert(false);
}

template<typename T_Arg, typename T_Result, typename T_Feedback>
void Server<T_Arg, T_Result, T_Feedback>::result(const CallID& call_id, const boost::shared_ptr<const T_Result>& msg)
{
  dispatcher_.lock()->returnResult(call_id, msg);
}

template<typename T_Arg, typename T_Result, typename T_Feedback>
void Server<T_Arg, T_Result, T_Feedback>::feedback(const CallID& call_id, const boost::shared_ptr<const T_Feedback>& msg)
{
  dispatcher_.lock()->returnFeedback(call_id, msg);
}

}
}
