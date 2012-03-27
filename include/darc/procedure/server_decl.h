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

#ifndef __DARC_PROCEDURE_SERVER_H_INCLUDED__
#define __DARC_PROCEDURE_SERVER_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <darc/log.h>
#include <darc/owner.h>
#include <darc/primitive.h>
#include <darc/enable_weak_from_static.h>
#include <darc/procedure/local_dispatcher_fwd.h>
#include <darc/procedure/id_types.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Result, typename T_Feedback>
class Server : public darc::Primitive
{
public:
  typedef boost::function<void(const CallID&, const T_Arg&)> MethodType;

protected:
  boost::asio::io_service * io_service_;
  std::string name_;
  boost::weak_ptr<LocalDispatcher<T_Arg, T_Result, T_Feedback> > dispatcher_;

  MethodType method_;

public:
  template<typename O>
  Server(O * owner, const std::string& name, void(O::*callback)(const CallID&, const T_Arg&)) :
    darc::Primitive(owner),
    io_service_(owner->getIOService()),
    name_(name),
    method_(boost::bind(callback, owner, _1, _2))
  {
    owner->addPrimitive(this);
  }

  // Called by darc::procedure::Dispatcher
  void postCall(const CallID& call_id, const boost::shared_ptr<const T_Arg>& argument)
  {
    io_service_->post( boost::bind(&Server::triggerCallHandler, this, call_id, argument) );
  }

  // Called by components
  void dispatchStatusMessage( boost::shared_ptr<T_Feedback>& msg )
  {
    dispatchStatusMessageLocally(msg);
  }

  void onStart();
  void onStop();
  void result(const CallID&, const boost::shared_ptr<const T_Result>& msg);
  void feedback(const CallID&, const boost::shared_ptr<const T_Feedback>& msg);

private:
  void triggerCallHandler(const CallID& call_id, boost::shared_ptr<const T_Arg> argument)
  {
    assert(method_);
    method_(call_id, *argument);
  }

};

}
}

#endif
