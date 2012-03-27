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

#include <map>
#include <darc/procedure/client_decl.h>
#include <darc/procedure/server_decl.h>
#include <darc/procedure/local_dispatcher_abstract.h>
#include <darc/procedure/remote_dispatcher.h>
#include <darc/procedure/manager_decl.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Result, typename T_Feedback>
class LocalDispatcher : public LocalDispatcherAbstract
{
protected:
  typedef Client<T_Arg, T_Result, T_Feedback> ClientType;
  typedef Server<T_Arg, T_Result, T_Feedback> ServerType;

  typedef std::map<ClientID, ClientType*> ClientListType;
  typedef std::map<CallID, NodeID> ActiveServerCallsType;
  typedef std::map<CallID, ClientID> ActiveClientCallsType;

protected:
  std::string procedure_name_;
  ProcedureID procedure_id_;

  Manager * manager_;

  ServerType* server_;
  ClientListType client_list_;

  // Structure to hold procedure calls we have received
  ActiveServerCallsType active_server_calls_;

  // Structure to hold procedure calls we have made
  ActiveClientCallsType active_client_calls_;

public:
  LocalDispatcher(const std::string& procedure_name, Manager * manager) :
    procedure_name_(procedure_name),
    procedure_id_(ProcedureID::create()),
    manager_(manager),
    server_(0)
  {
  }

  const ProcedureID& getProcedureID()
  {
    return procedure_id_;
  }

  // Called by Clients (Component Thread)
  const CallID& performCall(const ClientID& client_id, const boost::shared_ptr< T_Arg >& arg)
  {
    // Check if server is available locally
    if(server_ != 0)
    {
      // Create CallID
      CallID call_id = CallID::create();
      std::pair<ActiveClientCallsType::iterator, bool> element = active_client_calls_.insert(ActiveClientCallsType::value_type(call_id, client_id) );
      // Call
      dispatchCallLocally(call_id, CallID::null(), arg);
      return element.first->first;
    }
    else if(manager_->getRemoteDispatcher().hasRemoteServer(procedure_name_))
    {
      CallID call_id = CallID::create();
      std::pair<ActiveClientCallsType::iterator, bool> element = active_client_calls_.insert(ActiveClientCallsType::value_type(call_id, client_id) );
      manager_->getRemoteDispatcher().postRemoteCall<T_Arg>(procedure_id_, procedure_name_, call_id, arg);
      return element.first->first;
    }
    else
    {
      return CallID::null();
    }
  }

  // Called by Servers (Component Thread)
  void returnFeedback(const CallID& call_id, const boost::shared_ptr<const T_Feedback>& msg)
  {
    // Todo: do this a bit more efficient so local calls are faster
    ActiveServerCallsType::iterator server_call_entry = active_server_calls_.find(call_id);
    if(server_call_entry != active_server_calls_.end())
    {
      if(server_call_entry->second == NodeID::null()) //local
      {
	dispatchFeedbackLocally(call_id, msg);
      }
      else
      {
	manager_->getRemoteDispatcher().postRemoteFeedback(procedure_id_, call_id, server_call_entry->second /*NodeID*/, msg);
      }
    }
  }

  // Called by Servers (Component Thread)
  void returnResult(const CallID& call_id, const boost::shared_ptr<const T_Result>& msg)
  {
    // Todo: do this a bit more efficient so local calls are faster
    ActiveServerCallsType::iterator server_call_entry = active_server_calls_.find(call_id);
    if(server_call_entry != active_server_calls_.end())
    {
      if(server_call_entry->second == NodeID::null()) //local
      {
	active_server_calls_.erase(server_call_entry);
	dispatchResultLocally(call_id, msg);
      }
      else
      {
	manager_->getRemoteDispatcher().postRemoteResult(procedure_id_, call_id, server_call_entry->second /*NodeID*/, msg);
      }
    }
  }

  // Called by Clients (Component Thread)
  void registerClient( ClientType * client )
  {
    client_list_[client->getID()] = client;
  }

  // Called by Servers (Component Thread)
  void registerServer( ServerType * server )
  {
    assert(server_ == 0);
    manager_->getRemoteDispatcher().registerProcedure(procedure_name_,
						      procedure_id_,
						      ros::message_traits::DataType<T_Arg>::value(),
						      ros::message_traits::DataType<T_Result>::value(),
						      ros::message_traits::DataType<T_Feedback>::value());
    server_ = server;
  }

  // Called by Manager (Node Thread)
  void remoteCallReceived(SharedBuffer msg_s, const NodeID& remote_node_id, const CallID& call_id)
  {
    dispatchCallLocally(call_id,
			remote_node_id,
			Serialization::deserialize<T_Arg>(msg_s));
  }

  void remoteFeedbackReceived(SharedBuffer msg_s, const CallID& call_id)
  {
    dispatchFeedbackLocally(call_id,
			    Serialization::deserialize<T_Arg>(msg_s));
  }

  void remoteResultReceived(SharedBuffer msg_s, const CallID& call_id)
  {
    dispatchResultLocally(call_id,
			  Serialization::deserialize<T_Arg>(msg_s));
  }

private:
  void dispatchCallLocally(const CallID& call_id, const NodeID& calling_node, const boost::shared_ptr<const T_Arg>& arg)
  {
    if(server_ != 0)
    {
      active_server_calls_.insert(ActiveServerCallsType::value_type(call_id, calling_node));
      server_->postCall(call_id, arg);
    }
  }

  void dispatchFeedbackLocally(const CallID& call_id, const boost::shared_ptr<const T_Feedback>& msg)
  {
    ActiveClientCallsType::iterator client_call_entry = active_client_calls_.find(call_id);
    if(client_call_entry != active_client_calls_.end())
    {
      typename ClientListType::iterator client_entry = client_list_.find(client_call_entry->second);
      if(client_entry != client_list_.end())
      {
	client_entry->second->postFeedback(call_id, msg);
      }
    }
  }

  void dispatchResultLocally(const CallID& call_id, const boost::shared_ptr<const T_Feedback>& msg)
  {
    ActiveClientCallsType::iterator client_call_entry = active_client_calls_.find(call_id);
    if(client_call_entry != active_client_calls_.end())
    {
      typename ClientListType::iterator client_entry = client_list_.find(client_call_entry->second);
      active_client_calls_.erase(client_call_entry);
      if(client_entry != client_list_.end())
      {
	client_entry->second->postResult(call_id, msg);
      }
    }
  }

};

}
}
