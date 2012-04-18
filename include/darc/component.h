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
 * DARC Component class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_COMPONENT_H_INCLUDED__
#define __DARC_COMPONENT_H_INCLUDED__

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/scoped_ptr.hpp>
#include <darc/component_fwd.h>
#include <darc/owner.h>
#include <darc/enable_weak_from_static.h>
#include <darc/node.h>
#include <darc/id.h>
#include <darc/registry.h>
#include <darc/statistics/cpu_usage.h>
#include <darc/statistics/thread_statistics.h>

namespace darc
{

namespace python { class ComponentProxy; }

class Component : public Owner, public EnableWeakFromStatic<Component>
{
  friend class python::ComponentProxy;

private:
  std::string name_;
  NodePtr node_;
  bool attached_;
  boost::asio::io_service io_service_;
  boost::scoped_ptr<boost::asio::io_service::work> keep_alive_;
  ID id_;

  // Statistics
  statistics::CPUUsage cpu_usage_;
  statistics::ThreadStatistics statistics_;

protected:
  Component();
  void attachNode(const std::string& instance_name, NodePtr node);
  void triggerOnStart();

  virtual void onStart()
  {
  }

  void startProfilingHandler();
  void stopProfilingHandler();

public:
  void run();
  void stop();
  void pause();
  void unpause();

  void work();
  void stopWork();

  virtual void startProfiling();
  virtual void stopProfiling();

  // impl of darc::Owner
  inline boost::asio::io_service * getIOService()
  {
    return &io_service_;
  }

  inline const bool& isAttached()
  {
    return attached_;
  }

  // impl of darc::Owner
  inline NodePtr getNode()
  {
    assert(attached_);
    return node_;
  }

  // Getters
  inline const std::string getName() const
  {
    return name_;
  }

  inline const ID& getComponentID()
  {
    return id_;
  }

  inline const ID& getID()
  {
    return id_;
  }

  inline const statistics::ThreadStatistics& getStatistics()
  {
    return statistics_;
  }

  // Method to instantiate components
  template<typename T>
  static boost::shared_ptr<T> instantiate(const std::string& instance_name, NodePtr node)
  {
    boost::shared_ptr<T> instance(new T());
    instance->attachNode(instance_name, node);
    node->attach(instance);
    return instance;
  }

};

}

#endif
