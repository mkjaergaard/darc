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
 * DARC Procedure RemoteDispatcher impl
 *
 * \author Morten Kjaergaard
 */

#include <darc/serialization.h>
#include <darc/procedure/manager.h>
#include <darc/network/packet/header.h>
#include <darc/network/packet/procedure_call.h>
#include <darc/network/packet/procedure_advertise.h>
#include <darc/network/packet/procedure_result.h>
#include <darc/network/link_manager.h>
#include <darc/procedure/id_types.h>
#include <darc/log.h>

namespace darc
{
namespace procedure
{

RemoteDispatcher::RemoteDispatcher(boost::asio::io_service * io_service, Manager* manager, network::LinkManager * network_link_manager) :
  io_service_(io_service),
  manager_(manager),
  network_link_manager_(network_link_manager)
{
  network_link_manager->registerPacketReceivedHandler( network::packet::Header::PROCEDURE_ADVERTISE,
						       boost::bind( &RemoteDispatcher::advertiseReceiveHandler,
								    this, _1, _2, _3 ) );
  network_link_manager->registerPacketReceivedHandler( network::packet::Header::PROCEDURE_CALL,
						       boost::bind( &RemoteDispatcher::callReceiveHandler,
								    this, _1, _2, _3 ) );
  network_link_manager->registerPacketReceivedHandler( network::packet::Header::PROCEDURE_FEEDBACK,
						       boost::bind( &RemoteDispatcher::feedbackReceiveHandler,
								    this, _1, _2, _3 ) );
  network_link_manager->registerPacketReceivedHandler( network::packet::Header::PROCEDURE_RESULT,
						       boost::bind( &RemoteDispatcher::resultReceiveHandler,
								    this, _1, _2, _3 ) );
  network_link_manager->addNewRemoteNodeListener(boost::bind(&RemoteDispatcher::newRemoteNodeHandler,
							     this, _1));
}
void RemoteDispatcher::advertiseReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len )
{
  network::packet::ProcedureAdvertise packet;
  packet.read(buffer.data(), data_len);

  // Todo check the correct types
  AdvertisedProcedureInfo info(packet.procedure_name, packet.procedure_id);
  info.argument_type_name = packet.argument_type_name;
  info.feedback_type_name = packet.feedback_type_name;
  info.result_type_name = packet.result_type_name;
  info.advertising_node = header.sender_node_id;

  DARC_INFO("Received advertisement for procedure %s (%s)", info.procedure_name.c_str(), info.procedure_id.short_string().c_str());

  remote_procedures_.insert(RemoteAdvertisedProceduresType::value_type(packet.procedure_name, info));

  manager_->remoteProcedureAdvertiseChange(info);
}

void RemoteDispatcher::callReceiveHandler(const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len)
{
  network::packet::ProcedureCall packet;
  buffer.addOffset( packet.read(buffer.data(), data_len) );

  manager_->remoteCallReceived(packet.procedure_id, header.sender_node_id, packet.call_id, buffer);
}

void RemoteDispatcher::feedbackReceiveHandler(const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len)
{
  network::packet::ProcedureFeedback packet;
  buffer.addOffset( packet.read(buffer.data(), data_len) );

  RemoteActiveClientCallsType::iterator item = remote_active_client_calls_.find(packet.call_id);
  if(item != remote_active_client_calls_.end())
  {
    manager_->remoteFeedbackReceived(item->second /*local procedure_id*/, packet.call_id, buffer);
  }
  else
  {
    DARC_WARNING("Received feedback for unknown call_id %s", packet.call_id.short_string().c_str());
  }
}

void RemoteDispatcher::resultReceiveHandler(const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len)
{
  network::packet::ProcedureFeedback packet;
  buffer.addOffset( packet.read(buffer.data(), data_len) );

  RemoteActiveClientCallsType::iterator item = remote_active_client_calls_.find(packet.call_id);
  if(item != remote_active_client_calls_.end())
  {
    remote_active_client_calls_.erase(item);
    manager_->remoteResultReceived(item->second /*local procedure_id*/, packet.call_id, buffer);
  }
  else
  {
    DARC_WARNING("Received result for unknown call_id %s", packet.call_id.short_string().c_str());
  }
}

void RemoteDispatcher::newRemoteNodeHandler(const ID& remote_node_id)
{
  for(AdvertisedProceduresType::iterator it = advertised_procedures_.begin();
      it != advertised_procedures_.end();
      it++)
  {
    sendAdvertisement(it->second, remote_node_id);
  }
}

void RemoteDispatcher::registerProcedure(const std::string& procedure_name,
					 const ID& procedure_id,
					 const std::string& argument_type_name,
					 const std::string& feedback_type_name,
					 const std::string& result_type_name)
{
  // check that type is correct
  AdvertisedProcedureInfo item(procedure_name, procedure_id);
  item.argument_type_name = argument_type_name;
  item.feedback_type_name = feedback_type_name;
  item.result_type_name = result_type_name;
  advertised_procedures_.insert(AdvertisedProceduresType::value_type(procedure_id, item));
  sendAdvertisement(item, ID::null());
}

void RemoteDispatcher::sendAdvertisement(const AdvertisedProcedureInfo& info, const ID& remote_node_id)
{
  DARC_INFO("Sending advertisement to %s for procedure: %s (%s)",
	    remote_node_id.short_string().c_str(),
	    info.procedure_name.c_str(),
	    info.procedure_id.short_string().c_str());

  // Allocate buffer. todo: derive required size?
  std::size_t data_len = 1024;
  SharedBuffer buffer = SharedBufferArray::create(data_len);

  // Message Subscription Packet
  network::packet::ProcedureAdvertise packet;
  packet.procedure_name = info.procedure_name;
  packet.procedure_id = info.procedure_id;
  packet.argument_type_name = info.argument_type_name;
  packet.argument_type_name = info.argument_type_name;
  packet.argument_type_name = info.argument_type_name;
  packet.write(buffer.data(), buffer.size());

  network_link_manager_->sendPacket(network::packet::Header::PROCEDURE_ADVERTISE, remote_node_id, buffer, data_len);
}

void RemoteDispatcher::sendPacket(network::packet::Header::PayloadType type, const ID& recv_node_id, SharedBuffer buffer, std::size_t data_len)
{
  network_link_manager_->sendPacket(type, recv_node_id, buffer, data_len);
}

}
}
