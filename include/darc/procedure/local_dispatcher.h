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
 * DARC LocalDispatcher class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <vector>
#include <darc/procedure/client_decl.h>
#include <darc/procedure/server_decl.h>
#include <darc/procedure/local_dispatcher_abstract.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Result, typename T_Feedback>
class LocalDispatcher : public LocalDispatcherAbstract
{
private:
  std::string name_;

  typedef std::map<ID, boost::weak_ptr<Client<T_Arg, T_Result, T_Feedback> > > ClientListType;

  boost::weak_ptr<Server<T_Arg, T_Result, T_Feedback> > server_;
  ClientListType client_list_;

public:
  void dispatchCall( boost::shared_ptr< T_Arg >& arg )
  {
    dispatchCallLocally(arg);
  }

  void dispatchFeedback( boost::shared_ptr< T_Feedback >& msg )
  {
    dispatchFeedbackLocally(msg);
  }

  void dispatchResult( boost::shared_ptr< T_Result >& msg )
  {
    dispatchResultLocally(msg);
  }

  void registerClient( Client<T_Arg, T_Result, T_Feedback> * client )
  {
    client_list_[client->getID()] = client->getWeakPtr();
  }

  void registerServer( Server<T_Arg, T_Result, T_Feedback> * server )
  {
    assert(server_.use_count() == 0);
    server_ = server->getWeakPtr();
  }

private:
  void dispatchCallLocally( boost::shared_ptr<T_Feedback>& arg )
  {
    if( server_.use_count() != 0 )
    {
      server_.lock()->postCall(arg);
    }
  }

  void dispatchFeedbackLocally( boost::shared_ptr<T_Feedback>& msg )
  {
    // todo: only dispatch to the actual caller
    for( typename ClientListType::iterator it = client_list_.begin();
	 it != client_list_.end();
	 it++)
    {
      it->second.lock()->postFeedback(msg);
    }
  }

  void dispatchResultLocally( boost::shared_ptr<T_Feedback>& msg )
  {
    // todo: only dispatch to the actual caller
    for( typename ClientListType::iterator it = client_list_.begin();
	 it != client_list_.end();
	 it++)
    {
      it->second.lock()->postResult(msg);
    }
  }

};

}
}
