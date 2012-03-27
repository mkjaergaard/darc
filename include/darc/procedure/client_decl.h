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
 * DARC Client class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PROCEDURE_CLIENT_H_INCLUDED__
#define __DARC_PROCEDURE_CLIENT_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <darc/procedure/id_types.h>
#include <darc/procedure/local_dispatcher_fwd.h>
#include <darc/owner.h>
#include <darc/primitive.h>
#include <darc/enable_weak_from_static.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Result, typename T_Feedback>
class Client : public darc::Primitive
{
public:
  typedef boost::function<void(const CallID&, const T_Result&)> ResultHandlerType;
  typedef boost::function<void(const CallID&, const T_Feedback&)> FeedbackHandlerType;

protected:
  boost::asio::io_service * io_service_;
  std::string name_;
  ResultHandlerType result_handler_;
  FeedbackHandlerType feedback_handler_;

  boost::weak_ptr<LocalDispatcher<T_Arg, T_Result, T_Feedback> > dispatcher_;

public:
  template<typename O>
  Client(O * owner, const std::string& name, void(O::*callback)(const CallID&, const T_Result&)) :
    darc::Primitive(owner),
    io_service_(owner->getIOService()),
    name_(name),
    result_handler_(boost::bind(callback, owner, _1, _2))
  {
  }

  template<typename O>
  void registerFeedbackHandler(O * owner, void(O::*callback)(const CallID&, const T_Feedback&))
  {
    feedback_handler_ = boost::bind(callback, owner, _1, _2);
  }

  // Called by darc::procedure::LocalDispatcher
  void postFeedback(const CallID& call_id, const boost::shared_ptr<const T_Feedback>& msg)
  {
    if(feedback_handler_)
    {
      io_service_->post( boost::bind( &Client::triggerFeedbackHandler, this, call_id, msg ) );
    }
  }

  // Called by darc::procedure::LocalDispatcher
  void postResult(const CallID& call_id, const boost::shared_ptr<const T_Result>& msg)
  {
    if(result_handler_)
    {
      io_service_->post( boost::bind( &Client::triggerResultHandler, this, call_id, msg ) );
    }
  }

  const CallID& call( boost::shared_ptr<T_Arg>& argument );

protected:
  void onStart();
  void onStop();

  void triggerFeedbackHandler(const CallID call_id, const boost::shared_ptr<const T_Feedback> msg)
  {
    if(feedback_handler_)
    {
      feedback_handler_(call_id, *msg);
    }
  }

  void triggerResultHandler(const CallID call_id, const boost::shared_ptr<const T_Result> msg)
  {
    if(result_handler_)
    {
      result_handler_(call_id, *msg);
    }
  }

};

}
}

#endif
