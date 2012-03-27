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
 * DARC Owner class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_OWNER_H_INCLUDED__
#define __DARC_OWNER_H_INCLUDED__

#include <boost/asio/io_service.hpp>
#include <darc/primitive.h>
#include <darc/primitive_list.h>

namespace darc
{

namespace timer { class PeriodicTimer; }
namespace parameter { class ParameterAbstract; }

class Node;

class Owner
{
protected:
  PrimitiveList<timer::PeriodicTimer> timer_list_;
  PrimitiveList<parameter::ParameterAbstract> parameter_list_;
  PrimitiveList<Primitive> all_list_;

public:
  virtual boost::asio::io_service * getIOService() = 0;
  virtual boost::shared_ptr<darc::Node> getNode() = 0;
  virtual const ID& getComponentID() = 0;

  void startPrimitives()
  {
    all_list_.startAll();
  }

  void stopPrimitives()
  {
    all_list_.stopAll();
  }

  void pausePrimitives()
  {
    all_list_.pauseAll();
  }

  void unpausePrimitives()
  {
    all_list_.unpauseAll();
  }

  void latchStatistics(int32_t period_usec);

  void addPrimitive(boost::weak_ptr<Primitive> prim);
  void addTimer(boost::weak_ptr<timer::PeriodicTimer> timer);
  void addParameter(boost::weak_ptr<parameter::ParameterAbstract> parameter);

};

}

#endif
