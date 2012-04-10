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
 * DARC Composition class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_COMPOSITION_H_INCLUDED__
#define __DARC_COMPOSITION_H_INCLUDED__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <darc/node.h>
#include <darc/component.h>
#include <darc/primitive.h>
#include <darc/enable_weak_from_static.h>

namespace darc
{

class Composition : public Owner, public Primitive
{
protected:
  Owner * parent_;

protected:
  Composition(Owner * parent):
    Primitive(parent),
    parent_(parent)
  {
    parent->addPrimitive(this);
  }

  virtual ~Composition()
  {
  }

public:
  // impl of darc::Owner
  inline boost::asio::io_service * getIOService()
  {
    return parent_->getIOService();
  }

  inline const bool& isAttached()
  {
    return parent_->isAttached();
  }

  // impl of darc::Owner
  inline boost::shared_ptr<Node> getNode()
  {
    return parent_->getNode();
  }

  // impl of darc::Owner
  const ID& getComponentID()
  {
    return parent_->getComponentID();
  }

  // Override of Primitive methods
  virtual void start()
  {
    Owner::startPrimitives();
  }

  virtual void stop()
  {
    Owner::stopPrimitives();
  }

  virtual void pause()
  {
    Owner::pausePrimitives();
  }

  virtual void unpause()
  {
    Owner::unpausePrimitives();
  }

  virtual void onAttach()
  {
    triggerPrimitivesOnAttach();
  }

};

}

#endif
