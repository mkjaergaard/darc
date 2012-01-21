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
 * DARC Node class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <darc/id.h>
#include <darc/pubsub/fwd.h>
#include <darc/procedure/manager.h>

namespace darc
{

class Component;
typedef boost::shared_ptr<Component> ComponentPtr;

class Node
{
public:
  typedef boost::shared_ptr<Node> Ptr;

public:
  virtual void run(bool blocking = true) = 0;
  virtual pubsub::Manager& getPublisherManager() = 0;
  virtual procedure::Manager& getProcedureManager() = 0;
  virtual void accept( const std::string& url ) = 0;
  virtual void connect( uint32_t remote_node_id, const std::string& url ) = 0;
  virtual void setNodeID( uint32_t node_id ) = 0;
  virtual ComponentPtr instantiateComponent(const std::string& instance_name) = 0;
  virtual void runComponent(ID id) = 0;
  virtual void stopComponent(ID id) = 0;
  virtual void attach(ComponentPtr component) = 0;

  static Node::Ptr create();
};

typedef boost::shared_ptr<Node> NodePtr;

}
