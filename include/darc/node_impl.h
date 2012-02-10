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
#include <darc/pubsub/manager.h>
#include <darc/procedure/manager.h>
#include <darc/parameter/manager.h>
#include <darc/network/link_manager.h>

namespace darc
{

namespace python{ class NodeProxy; }

class NodeImpl : public Node, public boost::enable_shared_from_this<NodeImpl>
{
  friend class python::NodeProxy;

private:
  boost::asio::io_service io_service_;
  boost::thread node_thread_;
  ID node_id_;

  network::LinkManager node_link_manager_;
  ThreadManager thread_manager_;

  pubsub::Manager publish_manager_;
  procedure::Manager procedure_manager_;
  parameter::Manager parameter_manager_;

  typedef std::map<ID, ComponentPtr> ComponentInstancesList;
  ComponentInstancesList component_instances_;

public:
  NodeImpl() :
    node_id_(ID::create()),
    node_link_manager_(&io_service_, node_id_),
    publish_manager_(&io_service_, &node_link_manager_)
  {
    DARC_INFO("Created node with ID: %s", node_id_.short_string().c_str() );
  }

protected:
  void work()
  {
    DARC_INFO("Running Node with ID: %s ", node_id_.short_string().c_str() );
    boost::asio::io_service::work keep_alive(io_service_);
    io_service_.run();
  }

  void run(bool blocking = false)
  {
    node_thread_ = boost::thread( boost::bind(&NodeImpl::work, this) );
    if(blocking)
    {
      node_thread_.join();
    }
  }

  void attach(ComponentPtr component)
  {
    component_instances_[component->getID()] = component;
  }

  boost::shared_ptr<Component> instantiateComponent(const std::string& instance_name)
  {
    ComponentPtr c = Registry::instantiateComponent(instance_name, this->shared_from_this());
    component_instances_[c->getID()] = c;
    return c;
  }

  void runComponent(ID id)
  {
    assert(component_instances_.count(id) > 0);
    thread_manager_.allocateThreadAndRun(component_instances_[id]);
  }

  void stopComponent(ID id)
  {
    assert(component_instances_.count(id) > 0);
    thread_manager_.stopThread(component_instances_[id]);
  }

  pubsub::Manager& getPublisherManager()
  {
    return publish_manager_;
  }

  procedure::Manager& getProcedureManager()
  {
    return procedure_manager_;
  }

  parameter::Manager& getParameterManager()
  {
    return parameter_manager_;
  }

  void accept( const std::string& url )
  {
    node_link_manager_.accept(url);
  }

  void connect( const std::string& url )
  {
    node_link_manager_.connect( url);
  }

  const ID& lookupComponentInstance(const std::string& instance_name)
  {
    for( ComponentInstancesList::iterator it = component_instances_.begin(); it != component_instances_.end(); it++ )
    {
      if( it->second->getName() == instance_name )
      {
	return it->second->getComponentID();
      }
    }
    return ID::null();
  }

};

}

#endif
