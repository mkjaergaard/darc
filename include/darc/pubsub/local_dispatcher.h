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
#include <darc/pubsub/local_dispatcher_abstract.h>
#include <darc/pubsub/subscriber_decl.h>
#include <darc/pubsub/remote_dispatcher.h>
#include <darc/pubsub/i_manager_callback.h>

namespace darc
{
namespace pubsub
{

template<typename T>
class LocalDispatcher : public LocalDispatcherAbstract
{
private:
  std::string topic_;
  IManagerCallback * manager_callback_;

  typedef std::map<ID, boost::weak_ptr<Subscriber<T> > > SubscriberListType; // <-- weak_ptr! dont keep alive
  SubscriberListType subscriber_list_;

public:
  LocalDispatcher( const std::string& topic, IManagerCallback * manager_callback ) :
    topic_(topic),
    manager_callback_( manager_callback )
  {
  }

  void registerSubscriber( Subscriber<T> * sub )
  {
    // todo: higher level remote subscribe
    subscriber_list_[sub->getID(), sub->getWeakPtr()];
  }

  void unregisterSubscriber( Subscriber<T> * sub )
  {
    // todo: higher level remote subscribe
    subscriber_list_.erase(sub->getID());
  }

  // Called by the local publishers
  void dispatchMessage( boost::shared_ptr<T> msg )
  {
    dispatchMessageLocally(msg);
    // if remote subscribers
    manager_callback_->getRemoteDispatcher().postRemoteDispatch<T>(topic_, msg);
  }

  void dispatchMessageLocally( boost::shared_ptr<T> msg )
  {
    for( typename SubscriberListType::iterator it = subscriber_list_.begin();
         it != subscriber_list_.end();
         it++)
    {
      it->second.lock()->dispatch( msg );
    }
  }

  // impl of virtual
  void dispatchMessageLocally( SharedBuffer msg_s )
  {
    dispatchMessageLocally( Serialization::deserialize<T>(msg_s) );
  }

};

}
}

#endif
