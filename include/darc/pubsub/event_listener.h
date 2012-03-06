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

#pragma once

#include <boost/function.hpp>
#include <boost/signals.hpp>
#include <darc/primitive.h>
#include <darc/enable_weak_from_static.h>
#include <darc/owner.h>

namespace darc
{
namespace pubsub
{

class EventListener : public darc::Primitive, public darc::EnableWeakFromStatic<EventListener>
{
public:
  typedef boost::function<void(const std::string, const std::string, size_t)> RemoteSubscriberChangesCallbackType;
  RemoteSubscriberChangesCallbackType remote_subscriber_changes_callback_;

  typedef boost::function<void(const std::string, const std::string, size_t)> RemotePublisherChangesCallbackType;
  RemotePublisherChangesCallbackType remote_publisher_changes_callback_;

protected:
  darc::Owner * owner_;
  boost::signals::connection conn1_;
  boost::signals::connection conn2_;

public:
  EventListener(darc::Owner* owner);

  void remoteSubscriberChangesListen(RemoteSubscriberChangesCallbackType callback);
  void postRemoteSubscriberChanges(const std::string& topic, const std::string& type_name, size_t remote_subscribers);
  void triggerRemoteSubscriberChanges(const std::string topic, const std::string& type_name, size_t remote_subscribers);

  void remotePublisherChangesListen(RemoteSubscriberChangesCallbackType callback);
  void postRemotePublisherChanges(const std::string& topic, const std::string& type_name, size_t remote_publishers);
  void triggerRemotePublisherChanges(const std::string topic, const std::string& type_name, size_t remote_publisher);

};

}
}
