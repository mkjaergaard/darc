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
 * DARC Procedure RemoteDispatcher class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <map>
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <darc/serialization.h>
#include <darc/procedure/id_types.h>
#include <darc/procedure/advertised_procedure_info.h>
#include <darc/network/link_manager_fwd.h>
#include <darc/network/packet/header.h>
#include <darc/network/packet/procedure_call.h>
#include <darc/network/packet/procedure_advertise.h>
#include <darc/network/packet/procedure_feedback.h>
#include <darc/network/packet/procedure_result.h>
#include <darc/shared_buffer_array.h>
#include <darc/log.h>

namespace darc
{
namespace procedure
{

class Manager;

class RemoteDispatcher
{
private:
  boost::asio::io_service * io_service_;
  Manager * manager_;
  network::LinkManager * network_link_manager_;

  // Procedures we advertise
  typedef std::map<ProcedureID, AdvertisedProcedureInfo> AdvertisedProceduresType;
  AdvertisedProceduresType advertised_procedures_;

  // Procedures other nodes advertises
  typedef std::map<std::string, AdvertisedProcedureInfo> RemoteAdvertisedProceduresType;
  RemoteAdvertisedProceduresType remote_procedures_;

  typedef std::map<CallID, ProcedureID> RemoteActiveClientCallsType;
  RemoteActiveClientCallsType remote_active_client_calls_;

private:
  void newRemoteNodeHandler(const ID& remote_node_id);

public:
  RemoteDispatcher(boost::asio::io_service * io_service, Manager* manager, network::LinkManager * network_link_manager);

  void callReceiveHandler(const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len);
  void feedbackReceiveHandler(const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len);
  void resultReceiveHandler(const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len);
  void advertiseReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len );

  void registerProcedure(const std::string& procedure_name,
			 const ID& procedure_id,
			 const std::string& argument_type_name,
			 const std::string& feedback_type_name,
			 const std::string& result_type_name);
  void sendAdvertisement(const AdvertisedProcedureInfo& info, const ID& remote_node_id);

  bool hasRemoteServer(const std::string& procedure_name)
  {
    return (remote_procedures_.find(procedure_name) != remote_procedures_.end());
  }

  // Called by LocalDispatcher
  template<typename T_Arg>
  void postRemoteCall(const ProcedureID& local_procedure_id, /* fix this chaos */
		      const std::string& procedure_name,
		      const CallID& call_id,
		      const boost::shared_ptr<const T_Arg>& arg)
  {
    RemoteAdvertisedProceduresType::iterator item = remote_procedures_.find(procedure_name);
    if(item != remote_procedures_.end())
    {
      // Remeber who made the call
      remote_active_client_calls_.insert(RemoteActiveClientCallsType::value_type(call_id, local_procedure_id));

      io_service_->post(boost::bind(&RemoteDispatcher::serializeAndDispatchCall<T_Arg>,
				    this,
				    item->second.procedure_id,
				    call_id,
				    item->second.advertising_node, //Remote Node_ID
				    arg));
    }
  }

  template<typename T_Feedback>
  void postRemoteFeedback(const ProcedureID& procedure_id,
			  const CallID& call_id,
			  const NodeID& remote_node_id,
			  const boost::shared_ptr<const T_Feedback>& msg)
  {
    io_service_->post(boost::bind(&RemoteDispatcher::serializeAndDispatchFeedback<T_Feedback>,
				  this,
				  procedure_id,
				  call_id,
				  remote_node_id,
				  msg));
  }

  template<typename T_Result>
  void postRemoteResult(const ProcedureID& procedure_id,
			const CallID& call_id,
			const NodeID& remote_node_id,
			const boost::shared_ptr<const T_Result>& msg)
  {
    io_service_->post(boost::bind(&RemoteDispatcher::serializeAndDispatchResult<T_Result>,
				  this,
				  procedure_id,
				  call_id,
				  remote_node_id,
				  msg));
  }

private:
  void sendPacket(network::packet::Header::PayloadType type, const ID& recv_node_id, SharedBuffer buffer, std::size_t data_len);

  template<typename T_Arg>
  void serializeAndDispatchCall(const ProcedureID procedure_id,
				const CallID call_id,
				const NodeID remote_node,
				const boost::shared_ptr<const T_Arg> arg)
  {
    // Procedure Call Packet
    network::packet::ProcedureCall call_packet;
    call_packet.procedure_id = procedure_id;
    call_packet.call_id = call_id;

    std::size_t data_len = call_packet.size() + Serialization::size(arg);
    SharedBuffer buffer = SharedBufferArray::create(data_len);

    buffer.addOffset( call_packet.write(buffer.data(), buffer.size()) );
    Serialization::serialize<T_Arg>(buffer, arg);

    buffer.resetOffset();
    sendPacket(network::packet::Header::PROCEDURE_CALL,
	       remote_node,
	       buffer,
	       data_len);
  }

  template<typename T_Feedback>
  void serializeAndDispatchFeedback(const ProcedureID procedure_id,
				    const CallID call_id,
				    const NodeID remote_node,
				    const boost::shared_ptr<const T_Feedback> msg)
  {
    network::packet::ProcedureFeedback feedback_packet;
    feedback_packet.procedure_id = procedure_id;
    feedback_packet.call_id = call_id;

    std::size_t data_len = feedback_packet.size() + Serialization::size(msg);
    SharedBuffer buffer = SharedBufferArray::create(data_len);

    buffer.addOffset( feedback_packet.write(buffer.data(), buffer.size()) );
    Serialization::serialize<T_Feedback>(buffer, msg);

    buffer.resetOffset();
    sendPacket(network::packet::Header::PROCEDURE_FEEDBACK,
	       remote_node,
	       buffer,
	       data_len);
  }

  template<typename T_Result>
  void serializeAndDispatchResult(const ProcedureID procedure_id,
				    const CallID call_id,
				    const NodeID remote_node,
				    const boost::shared_ptr<const T_Result> msg)
  {
    network::packet::ProcedureResult packet;
    packet.procedure_id = procedure_id;
    packet.call_id = call_id;

    std::size_t data_len = packet.size() + Serialization::size(msg);
    SharedBuffer buffer = SharedBufferArray::create(data_len);

    buffer.addOffset( packet.write(buffer.data(), buffer.size()) );
    Serialization::serialize<T_Result>(buffer, msg);

    buffer.resetOffset();
    sendPacket(network::packet::Header::PROCEDURE_RESULT,
	       remote_node,
	       buffer,
	       data_len);
  }

};

typedef boost::shared_ptr<RemoteDispatcher> RemoteDispatcherPtr;

}
}
