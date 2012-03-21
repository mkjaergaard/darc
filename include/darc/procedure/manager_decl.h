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
 * DARC Manager class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PROCEDURE_MANAGER_DECL_INCLUDED__
#define __DARC_PROCEDURE_MANAGER_DECL_INCLUDED__

#include <map>
#include <boost/shared_ptr.hpp>
#include <darc/procedure/local_dispatcher_abstract.h>
#include <darc/procedure/local_dispatcher_fwd.h>
#include <darc/procedure/remote_dispatcher.h>
#include <darc/procedure/advertised_procedure_info.h>

namespace darc
{
namespace procedure
{

class Manager
{
private:
  // Mapping from ProcedureName to ProcedureID
  typedef std::map<const std::string, ProcedureID> ProcedureIDListType;
  ProcedureIDListType procedure_id_list_;

  // Collection of LocalDispatcher instances (Mapped to ProcedureID)
  typedef std::map<ProcedureID, LocalDispatcherAbstractPtr> DispatcherListType;
  DispatcherListType dispatcher_list_;

  // Single instance of RemoteDispatcher
  RemoteDispatcher remote_dispatcher_;

  // Signals
  typedef boost::signal<void (const AdvertisedProcedureInfo&, const size_t&)> RemoteProcedureChangeSignalType;
  RemoteProcedureChangeSignalType remote_procedure_change_signal_;

public:
  // Constructor
  Manager(boost::asio::io_service * io_service, network::LinkManager * link_manager);

  //
  inline RemoteDispatcher& getRemoteDispatcher()
  {
    return remote_dispatcher_;
  }

  inline RemoteProcedureChangeSignalType& getRemoteProcedureChangeSignal()
  {
    return remote_procedure_change_signal_;
  }

  // Called by RemoteDispatcher (Node Thread)
  void remoteProcedureAdvertiseChange(const AdvertisedProcedureInfo& procedure_info); // plus add info if is also removed
  void remoteCallReceived(const ProcedureID& procedure_id, const NodeID& remote_node_id, const CallID& call_id, SharedBuffer msg);
  void remoteFeedbackReceived(const ProcedureID& procedure_id, const CallID& call_id, SharedBuffer msg);
  void remoteResultReceived(const ProcedureID& procedure_id, const CallID& call_id, SharedBuffer msg);

  // Called by Client/Servers (Component Threads)
  template<typename T_Arg, typename T_Result, typename T_Feedback>
  boost::shared_ptr<LocalDispatcher<T_Arg, T_Result, T_Feedback> > getLocalDispatcher(const std::string& name);

};

}
}

#endif
