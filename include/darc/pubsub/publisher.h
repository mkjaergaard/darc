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
 * DARC Publisher class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <boost/smart_ptr.hpp>
#include <darc/node.h>
#include <darc/primitive.h>
#include <darc/pubsub/manager.h>
#include <darc/enable_weak_from_static.h>
#include <darc/owner.h>

namespace darc
{
namespace pubsub
{

template<typename T>
class Publisher : public darc::Primitive
{
protected:
  boost::weak_ptr<LocalDispatcher<T> > dispatcher_;
  std::string topic_;

protected:
  const char * getTypeName()
  {
    return "Publisher";
  }

  const uint32_t getTypeID()
  {
    return 0x00010001u;
  }

public:
  Publisher(darc::Owner* owner, const std::string& topic) :
    darc::Primitive(owner),
    topic_(topic)
  {
  }

  void publish(boost::shared_ptr<const T> msg)
  {
    if(boost::shared_ptr<LocalDispatcher<T> > dispatcher_sp = dispatcher_.lock())
    {
      ID someid = owner_->getComponentID();;
      dispatcher_sp->dispatchMessage(msg, someid);
    }
  }

  void onStart()
  {
    dispatcher_ = owner_->getNode()->getPublisherManager().getLocalDispatcher<T>(topic_);
    dispatcher_.lock()->registerPublisher();
  }

  void onStop()
  {
    //todo: unregister
  }

};

}
}
