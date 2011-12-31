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
#include <darc/serialized_message.h>
#include <darc/publish/local_dispatcher_abstract.h>
#include <darc/publish/subscriber_impl.h>
#include <darc/publish/remote_dispatcher.h>

namespace darc
{
namespace publish
{

template<typename T>
class LocalDispatcher : public LocalDispatcherAbstract
{
private:
  std::string topic_;
  RemoteDispatcher * remote_dispatcher_;

  typedef std::vector< boost::shared_ptr<SubscriberImpl<T> > > SubscriberListType; // <-- weak_ptr
  SubscriberListType subscriber_list_;

public:
  LocalDispatcher( const std::string& topic, RemoteDispatcher * remote_dispatcher ) :
    topic_(topic),
    remote_dispatcher_( remote_dispatcher )
  {
  }

  void registerSubscriber( boost::shared_ptr<SubscriberImpl<T> > sub )
  {
    subscriber_list_.push_back( sub );
  }

  // Called by the local publishers
  void dispatchMessage( boost::shared_ptr<T> msg )
  {
    dispatchMessageLocally(msg);
    remote_dispatcher_->postRemoteDispatch<T>(topic_, msg);
  }

  void dispatchMessageLocally( boost::shared_ptr<T> msg )
  {
    for( typename SubscriberListType::iterator it = subscriber_list_.begin();
         it != subscriber_list_.end();
         it++)
    {
      (*it)->dispatch( msg );
    }
  }

  // impl of virtual
  void dispatchMessageLocally( SerializedMessage::ConstPtr msg_s )
  {
    dispatchMessageLocally( msg_s->deserialize<T>() );
  }

};

}
}

#endif
