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
 * DARC RemoteDispatcherManager impl
 *
 * \author Morten Kjaergaard
 */

#include <darc/pubsub/remote_dispatcher.h>
#include <darc/pubsub/manager.h>
#include <darc/network/link_manager.h>

namespace darc
{
namespace pubsub
{

void RemoteDispatcher::sendPacket(network::packet::Header::PayloadType type, const ID& recv_node_id, SharedBuffer buffer, std::size_t data_len)
{
  network_link_manager_->sendPacket(type, recv_node_id, buffer, data_len);
}

RemoteDispatcher::RemoteDispatcher(boost::asio::io_service * io_service, Manager * manager, network::LinkManager * network_link_manager) :
  io_service_(io_service),
  manager_(manager),
  network_link_manager_(network_link_manager)
{
  network_link_manager_->registerPacketReceivedHandler( network::packet::Header::MSG_PACKET,
							boost::bind( &RemoteDispatcher::messageReceiveHandler,
								     this, _1, _2, _3 ) );
  network_link_manager_->registerPacketReceivedHandler( network::packet::Header::MSG_SUBSCRIBE,
							boost::bind( &RemoteDispatcher::subscriptionReceiveHandler,
								     this, _1, _2, _3 ) );
  network_link_manager_->registerPacketReceivedHandler( network::packet::Header::MSG_PUBLISH_INFO,
							boost::bind( &RemoteDispatcher::publishInfoReceiveHandler,
								     this, _1, _2, _3 ) );
  network_link_manager_->addNewRemoteNodeListener(boost::bind(&RemoteDispatcher::newRemoteNodeHandler,
							      this, _1));
}

void RemoteDispatcher::messageReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len )
{
  // Parse Msg Packet
  network::packet::Message msg_packet;
  size_t msg_header_size = msg_packet.read( buffer.data(), data_len );
  buffer.addOffset( msg_header_size );

  DARC_TRACE("Received message for topic: %s, %u", msg_packet.topic.c_str(), msg_header_size);

  // Dispatch to local subscribers
  manager_->remoteMessageReceived(msg_packet.topic, buffer);
}

void RemoteDispatcher::subscriptionReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len )
{
  // Parse Packet
  network::packet::MessageSubscribe packet;
  packet.read( buffer.data(), data_len );

  // Todo check the correct type
  remote_subscribers_.insert(RemoteSubscribersType::value_type(packet.topic, header.sender_node_id));

  // Trigger Signal
  remote_pubsub_change_signal_(packet.topic,
			       packet.type_name,
			       remote_subscribers_.count(packet.topic),
			       remote_publishers_.count(packet.topic));
}

void RemoteDispatcher::publishInfoReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len )
{
  // Parse Packet
  network::packet::MessagePublishInfo packet;
  packet.read( buffer.data(), data_len );

  // Todo check the correct type
  remote_publishers_.insert(RemoteSubscribersType::value_type(packet.topic, header.sender_node_id));

  // Trigger Signal
  remote_pubsub_change_signal_(packet.topic,
			       packet.type_name,
			       remote_subscribers_.count(packet.topic),
			       remote_publishers_.count(packet.topic));
}

void RemoteDispatcher::newRemoteNodeHandler(const ID& remote_node_id)
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

void RemoteDispatcher::registerSubscription(const std::string& topic, const std::string& type_name)
{
  // check that type is correct
  SubscribedTopicInfo item(topic, type_name);
  subscribed_topics_.insert(item);
  sendSubscription(topic, type_name, ID::null());
}

void RemoteDispatcher::registerPublisher(const std::string& topic, const std::string& type_name)
{
  // check that type is correct
  SubscribedTopicInfo item(topic, type_name);
  published_topics_.insert(item);
  sendPublish(topic, type_name, ID::null());
}

void RemoteDispatcher::sendSubscription(const std::string& topic, const std::string& type_name, const ID& remote_node_id)
{
  DARC_DEBUG("Sending subscription to %s for topic: %s", remote_node_id.short_string().c_str(), topic.c_str());

  // Allocate buffer. todo: derive required size?
  std::size_t data_len = 1024;
  SharedBuffer buffer = SharedBufferArray::create(data_len);

  // Message Subscription Packet
  network::packet::MessageSubscribe packet;
  packet.topic = topic;
  packet.type_name = type_name;
  packet.write(buffer.data(), buffer.size());

  sendPacket(network::packet::Header::MSG_SUBSCRIBE, remote_node_id, buffer, data_len);
}

void RemoteDispatcher::sendPublish(const std::string& topic, const std::string& type_name, const ID& remote_node_id)
{
  DARC_DEBUG("Sending publish info to %s for topic: %s", remote_node_id.short_string().c_str(), topic.c_str());

  // Allocate buffer. todo: derive required size?
  std::size_t data_len = 1024;
  SharedBuffer buffer = SharedBufferArray::create(data_len);

  // Message Subscription Packet
  network::packet::MessagePublishInfo packet;
  packet.topic = topic;
  packet.type_name = type_name;
  packet.write(buffer.data(), buffer.size());

  sendPacket( network::packet::Header::MSG_PUBLISH_INFO, remote_node_id, buffer, data_len );
}

}
}
