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
 * DARC Event Listener class
 *
 * \author Morten Kjaergaard
 */

#include <darc/pubsub/state_interface.h>

#include <darc/node.h>
#include <darc/pubsub/manager.h>

namespace darc
{
namespace pubsub
{

StateInterface::StateInterface(darc::Owner* owner):
  Primitive(owner)
{
}

void StateInterface::onAttach()
{
  conn1_ = owner_->getNode()->getPublisherManager().getRemoteDispatcher().remoteSubscriberChangeSignal().
    connect(boost::bind(&StateInterface::postRemoteSubscriberChanges,
			this, _1, _2, _3));
  conn2_ = owner_->getNode()->getPublisherManager().getRemoteDispatcher().remotePublisherChangeSignal().
    connect(boost::bind(&StateInterface::postRemotePublisherChanges,
			this, _1, _2, _3));
}

// ******************
// SubscriberChanges
// ******************
void StateInterface::remoteSubscriberChangesListen(RemoteSubscriberChangesCallbackType callback)
{
  remote_subscriber_changes_callback_ = callback;
}

void StateInterface::postRemoteSubscriberChanges(const std::string& topic, const std::string& type_name, size_t remote_subscribers)
{
  owner_->getIOService()->post( boost::bind(&StateInterface::triggerRemoteSubscriberChanges, this, topic, type_name, remote_subscribers) );
}

void StateInterface::triggerRemoteSubscriberChanges(const std::string topic, const std::string& type_name, size_t remote_subscribers)
{
  remote_subscriber_changes_callback_(topic, type_name, remote_subscribers);
}

// ******************
// PublisherChanges
// ******************
void StateInterface::remotePublisherChangesListen(RemotePublisherChangesCallbackType callback)
{
  remote_publisher_changes_callback_ = callback;
}

void StateInterface::postRemotePublisherChanges(const std::string& topic, const std::string& type_name, size_t remote_publishers)
{
  owner_->getIOService()->post( boost::bind(&StateInterface::triggerRemotePublisherChanges, this, topic, type_name, remote_publishers) );
}

void StateInterface::triggerRemotePublisherChanges(const std::string topic, const std::string& type_name, size_t remote_publishers)
{
  remote_subscriber_changes_callback_(topic, type_name, remote_publishers);
}

}
}
