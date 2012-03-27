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

#ifndef __DARC_PUBLISH_LOCAL_DISPATCHER_H_INCLUDED___
#define __DARC_PUBLISH_LOCAL_DISPATCHER_H_INCLUDED___

#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <darc/id.h>
#include <darc/pubsub/subscriber_decl.h>
#include <darc/pubsub/local_dispatcher_abstract.h>
#include <darc/pubsub/remote_dispatcher.h>
#include <darc/pubsub/manager_decl.h>

namespace darc
{
namespace pubsub
{

template<typename T>
class LocalDispatcher : public LocalDispatcherAbstract
{
private:
  std::string topic_;
  Manager * manager_;

  typedef std::map<ID, Subscriber<T>* > SubscriberListType;
  SubscriberListType subscriber_list_;

  size_t publisher_count_;

public:
  LocalDispatcher(const std::string& topic, Manager * manager) :
    topic_(topic),
    manager_(manager),
    publisher_count_(0)
  {
  }

  ~LocalDispatcher()
  {
    // todo: check for outliving subscribers/publishers
  }

  void registerSubscriber( Subscriber<T> * sub )
  {
    if( subscriber_list_.empty() )
    {
      manager_->getRemoteDispatcher().registerSubscription(topic_, ros::message_traits::DataType<T>::value());
    }
    subscriber_list_[sub->getID()] = sub;
  }

  void unregisterSubscriber( Subscriber<T> * sub )
  {
    // todo: higher level remote subscribe
    subscriber_list_.erase(sub->getID());
    if( subscriber_list_.empty() )
    {
      //manager_callback_->getRemoteDispatcher().unregisterSubscription(topic_):
    }
  }

  void registerPublisher()
  {
    if(publisher_count_ == 0)
    {
      manager_->getRemoteDispatcher().registerPublisher(topic_, ros::message_traits::DataType<T>::value());
    }
    publisher_count_++;
  }

  void unregisterPublisher()
  {
    publisher_count_--;
  }

  // Called by the local publishers
  void dispatchMessage( boost::shared_ptr<const T> &msg, darc::ID &sender_component_id )
  {
    CallbackInfo info;
    info.sender_component_id = sender_component_id;
    dispatchMessageLocally(msg, info);
    // if remote subscribers
    manager_->getRemoteDispatcher().postRemoteDispatch<T>(topic_, msg);
  }

  void dispatchMessageLocally( boost::shared_ptr<const T> msg, CallbackInfo info )
  {
    for( typename SubscriberListType::iterator it = subscriber_list_.begin();
         it != subscriber_list_.end();
         it++)
    {
      it->second->dispatch( msg, info );
    }
  }

  // impl of virtual
  void dispatchMessageLocally( SharedBuffer msg_s )
  {
    dispatchMessageLocally( Serialization::deserialize<T>(msg_s), CallbackInfo() );
  }

};

}
}

#endif
