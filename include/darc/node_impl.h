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
 * DARC Node_Impl class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_NODE_IMPL_H_INCLUDED__
#define __DARC_NODE_IMPL_H_INCLUDED__

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <darc/node.h>
#include <darc/component.h>
#include <darc/thread_manager.h>
#include <darc/node_link_manager.h>
#include <darc/pubsub/manager.h>
#include <darc/procedure/manager.h>

namespace darc
{

namespace python{ class NodeProxy; }

class NodeImpl : public Node, public boost::enable_shared_from_this<NodeImpl>
{
  friend class python::NodeProxy;

private:
  boost::asio::io_service io_service_;
  boost::thread node_thread_;

  NodeLinkManager node_link_manager_;
  ThreadManager thread_manager_;

  pubsub::Manager publish_manager_;
  procedure::Manager procedure_manager_;

  typedef std::map<ID, ComponentPtr> ComponentInstancesList;
  ComponentInstancesList component_instances_;

public:
  NodeImpl() :
    node_link_manager_(&io_service_),
    publish_manager_(&io_service_, &node_link_manager_)
  {
  }

private:
  void run()
  {
    std::cout << "Running Node with ID " << node_link_manager_.getNodeID() << std::endl;
    boost::asio::io_service::work keep_alive(io_service_);
    io_service_.run();
  }

  boost::shared_ptr<Component> instantiateComponent(const std::string& instance_name)
  {
    ComponentPtr c = Registry::instantiateComponent(instance_name, this->shared_from_this());
    component_instances_[c->getID()] = c;
    return c;
  }

  void runComponent(ID id)
  {
    thread_manager_.allocateThreadAndRun(component_instances_[id]);
  }

  pubsub::Manager& getPublisherManager()
  {
    return publish_manager_;
  }

  procedure::Manager& getProcedureManager()
  {
    return procedure_manager_;
  }

  void setNodeID( uint32_t node_id )
  {
    node_link_manager_.setNodeID(node_id);
  }

  void accept( const std::string& url )
  {
    node_link_manager_.accept(url);
  }

  void connect( uint32_t remote_node_id, const std::string& url )
  {
    node_link_manager_.connect(remote_node_id, url);
  }


};

}

#endif
