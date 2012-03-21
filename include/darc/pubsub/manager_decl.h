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
 * DARC LocalDispatcherManager class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PUBLISH_MANAGER_DECL_H_INCLUDED__
#define __DARC_PUBLISH_MANAGER_DECL_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <darc/pubsub/local_dispatcher_abstract.h>
#include <darc/pubsub/local_dispatcher_fwd.h>
#include <darc/pubsub/remote_dispatcher.h>
#include <darc/network/link_manager_fwd.h>

namespace darc
{
namespace pubsub
{

class Manager
{
private:
  // Collection of LocalDispatcher instaces (Mapped to topic)
  typedef std::map<const std::string, LocalDispatcherAbstractPtr > LocalDispatcherListType;
  LocalDispatcherListType local_dispatcher_list_;

  // We hold the instance of RemoteDispatcher
  RemoteDispatcher remote_dispatcher_;

public:
  // Constructor
  Manager(boost::asio::io_service * io_service, network::LinkManager * node_link_manager);

  //
  inline RemoteDispatcher& getRemoteDispatcher()
  {
    return remote_dispatcher_;
  }

  // Called by RemoteDispatcher (Node Thread)
  void remoteMessageReceived(const std::string& topic, SharedBuffer msg_s);

  // Called by Publishers/Subscribers/RemoteDispatcher (Node and Component Threads)
  template<typename T>
  boost::shared_ptr<LocalDispatcher<T> > getLocalDispatcher(const std::string& topic);

};

}
}

#endif