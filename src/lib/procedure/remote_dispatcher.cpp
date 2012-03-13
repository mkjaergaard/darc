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
  network_link_manager->registerPacketReceivedHandler( network::packet::Header::PROCEDURE_CALL,
						       boost::bind( &RemoteDispatcher::callReceiveHandler,
								    this, _1, _2, _3 ) );
  network_link_manager->registerPacketReceivedHandler( network::packet::Header::PROCEDURE_ADVERTISE,
						       boost::bind( &RemoteDispatcher::advertiseReceiveHandler,
								    this, _1, _2, _3 ) );
  //    link_manager->addNewRemoteNodeListener(boost::bind(&RemoteDispatcher::newRemoteNodeHandler,
  //						       this, _1));
}

void RemoteDispatcher::callReceiveHandler(const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len)
{
  network::packet::ProcedureCall packet;
  size_t packet_size = packet.read(buffer.data(), data_len);
  buffer.addOffset(packet_size);

  //    local_dispatch_call_function_(packet.procedure_id, packet.call_id, buffer);
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

  RemoteAdvertisedProcedureInfo remote_info(header.sender_node_id, info);

  remote_procedures_.insert(RemoteAdvertisedProceduresType::value_type(packet.procedure_name, remote_info));

  // Trigger Signal
  //    signal_remote_subscriber_change_(packet.topic, packet.type_name, remote_subscribers_.count(packet.topic));
}

/*
  void newRemoteNodeHandler(const ID& remote_node_id)
  {
  for(SubscribedTopicsType::iterator it = subscribed_topics_.begin();
  it != subscribed_topics_.end();
  it++)
  {
  sendSubscription(it->topic, it->type_name, remote_node_id);
  }
  for(SubscribedTopicsType::iterator it = published_topics_.begin();
  it != published_topics_.end();
  it++)
  {
  sendPublish(it->topic, it->type_name, remote_node_id);
  }
  }
*/
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
  advertised_procedures_.insert(item);
  //sendSubscription(topic, type_name, ID::null());
}

void RemoteDispatcher::sendAdvertisement(const AdvertisedProcedureInfo& info, const ID& remote_node_id)
{
  DARC_INFO("Sending advertisement to %s for procedure: %s", remote_node_id.short_string().c_str(), info.procedure_name.c_str());

  // Allocate buffer. todo: derive required size?
  std::size_t data_len = 1024;
  SharedBuffer buffer = SharedBuffer::create(data_len);

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

/*
// Triggered by asio post
template<typename T>
void serializeAndDispatch( const std::string topic, const boost::shared_ptr<const T> msg )
{
// Message Header
network::packet::Message msg_header(topic);

// Allocate buffer
std::size_t data_len = ros::serialization::serializationLength(*msg)
+ msg_header.size()
+ 16 // MD5
+ strlen(ros::message_traits::DataType<T>::value()) + 1;

SharedBuffer buffer = SharedBuffer::create(data_len);

std::size_t pos = msg_header.write( buffer.data(), buffer.size() );

// todo: Put common serialization stuff somewhere to reuse
// Write Type Info
pos += network::packet::Parser::writeString( ros::message_traits::DataType<T>::value(), buffer.data() + pos, buffer.size() - pos );
// MD5
pos += network::packet::Parser::writeUint64( ros::message_traits::MD5Sum<T>::static_value1, buffer.data() + pos, buffer.size() - pos );
pos += network::packet::Parser::writeUint64( ros::message_traits::MD5Sum<T>::static_value2, buffer.data() + pos, buffer.size() - pos );

// Serialize actual message
ros::serialization::OStream ostream( buffer.data() + pos, buffer.size() - pos );
ros::serialization::serialize( ostream, *(msg.get()) );

assert( send_to_node_function_ );

// Find the subscribers
RemoteSubscribersRangeType subscribers = remote_subscribers_.equal_range(topic);
for(RemoteSubscribersType::iterator it = subscribers.first; it != subscribers.second; it++)
{
send_to_node_function_( network::packet::Header::MSG_PACKET, it->second, buffer, data_len );
}
}

// Called by LocalDispatcher
template<typename T>
void postRemoteDispatch( const std::string& topic, const boost::shared_ptr<const T> msg )
{
if(remote_subscribers_.count(topic) != 0)
{
io_service_->post( boost::bind(&RemoteDispatcher::serializeAndDispatch<T>, this, topic, msg) );
}
}
*/

}
}
