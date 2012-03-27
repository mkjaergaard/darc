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

#include <darc/component.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <darc/log.h>

namespace darc
{

Component::Component() :
  name_(""),
  attached_(false),
  id_(ID::create()),
  statistics_period_(boost::posix_time::seconds(1)),
  statistics_timer_(io_service_, statistics_period_)
{
}

void Component::attachNode(const std::string& instance_name, NodePtr node)
{
  attached_ = true;
  name_ = instance_name;
  node_ = node;
}

void Component::statisticsTimerHandler(const boost::system::error_code& error)
{
  if(!error)
  {
    statistics_timer_.expires_from_now(statistics_period_);
    statistics_timer_.async_wait(boost::bind( &Component::statisticsTimerHandler, this, boost::asio::placeholders::error ));
    latchStatistics( statistics_period_.total_milliseconds() );
  }
}

void Component::onStart()
{
  statistics_timer_.expires_from_now(statistics_period_);
  statistics_timer_.async_wait(boost::bind( &Component::statisticsTimerHandler, this, boost::asio::placeholders::error ));
}

void Component::run()
{
  assert(attached_);
  node_->runComponent(id_);
}

void Component::stop()
{
  assert(attached_);
  node_->stopComponent(id_);
}

void Component::pause()
{
  pausePrimitives();
}

void Component::unpause()
{
  unpausePrimitives();
}

void Component::work()
{
  DARC_INFO("Running Component: %s", name_.c_str());
  keep_alive_.reset( new boost::asio::io_service::work(io_service_) );
  startPrimitives();
  onStart();
  io_service_.reset();
  io_service_.run();
  DARC_INFO("Stopped Component: %s", name_.c_str());
}

void Component::stopWork()
{
  stopPrimitives();
  keep_alive_.reset();
}

}