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
 * DARC RemoteDispatcherManager class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PUBLISH_REMOTE_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_PUBLISH_REMOTE_DISPATCH_HANDLER_H_INCLUDED__

#include <map>
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <darc/serialization.h>
//#include <darc/pubsub/manager_fwd.h>
#include <darc/network/link_manager_fwd.h>
#include <darc/network/packet/header.h>
#include <darc/network/packet/message.h>
#include <darc/network/packet/message_subscribe.h>
#include <darc/network/packet/message_publish_info.h>
#include <darc/log.h>

namespace darc
{
namespace pubsub
{

class Manager;

class RemoteDispatcher
{
private:
  struct SubscribedTopicInfo
  {
    std::string topic;
    std::string type_name;

    bool operator<(const SubscribedTopicInfo& other) const
    {
      return topic < other.topic;
    }

    SubscribedTopicInfo(const std::string& topic, const std::string& type_name) :
      topic(topic),
      type_name(type_name)
    {
    }
  };

private:
  boost::asio::io_service * io_service_;
  Manager * manager_;
  network::LinkManager * network_link_manager_;

  // Topics we subscribe to
  typedef std::set<SubscribedTopicInfo> SubscribedTopicsType;
  SubscribedTopicsType subscribed_topics_;

  // Topics we publish (todo: tmp hack, not really required by the msg system, should be handled differently)
  typedef std::set<SubscribedTopicInfo> PublishedTopicsType;
  PublishedTopicsType published_topics_;

  // Topics other subscribe to
  typedef std::multimap<std::string, ID> RemoteSubscribersType;
  typedef std::pair<RemoteSubscribersType::iterator, RemoteSubscribersType::iterator> RemoteSubscribersRangeType;
  RemoteSubscribersType remote_subscribers_;

  // Topics other publishes to (todo: tmp hack, also handle differnetly)
  typedef std::multimap<std::string, ID> RemotePublishersType;
  typedef std::pair<RemotePublishersType::iterator, RemotePublishersType::iterator> RemotePublishersRangeType;
  RemotePublishersType remote_publishers_;

  // Signals
  boost::signal<void (const std::string&, const std::string&, size_t)> signal_remote_subscriber_change_;
  boost::signal<void (const std::string&, const std::string&, size_t)> signal_remote_publisher_change_;

private:
  void sendPacket(network::packet::Header::PayloadType type, const ID& recv_node_id, SharedBuffer buffer, std::size_t data_len);

public:
  RemoteDispatcher(boost::asio::io_service * io_service, Manager * manager, network::LinkManager * network_link_manager);

  boost::signal<void (const std::string&, const std::string&, size_t)>& remoteSubscriberChangeSignal()
  {
    return signal_remote_subscriber_change_;
  }

  boost::signal<void (const std::string&, const std::string&, size_t)>& remotePublisherChangeSignal()
  {
    return signal_remote_publisher_change_;
  }

  void messageReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len );
  void subscriptionReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len );
  void publishInfoReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len );

  void newRemoteNodeHandler(const ID& remote_node_id);

  void registerSubscription(const std::string& topic, const std::string& type_name);
  void registerPublisher(const std::string& topic, const std::string& type_name);

  void sendSubscription(const std::string& topic, const std::string& type_name, const ID& remote_node_id);
  void sendPublish(const std::string& topic, const std::string& type_name, const ID& remote_node_id);

  // Triggered by asio post
  template<typename T>
  void serializeAndDispatch( const std::string topic, const boost::shared_ptr<const T> msg )
  {
    // Message Header
    network::packet::Message msg_header(topic);

    // Allocate buffer
    std::size_t data_len = ros::serialization::serializationLength(*msg)
      + msg_header.size()
      + 16/*MD5*/
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

    // Find the subscribers
    RemoteSubscribersRangeType subscribers = remote_subscribers_.equal_range(topic);
    for(RemoteSubscribersType::iterator it = subscribers.first; it != subscribers.second; it++)
    {
      sendPacket( network::packet::Header::MSG_PACKET, it->second, buffer, data_len );
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

};

typedef boost::shared_ptr<RemoteDispatcher> RemoteDispatcherPtr;

}
}

#endif
