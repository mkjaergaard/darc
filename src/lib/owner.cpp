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
 * DARC Owner class implementation
 *
 * \author Morten Kjaergaard
 */

#include <darc/owner.h>
#include <darc/timer/periodic_timer.h>
#include <darc/parameter/parameter.h>
#include <darc/composition.h>

namespace darc
{

void Owner::addPrimitive(boost::weak_ptr<Primitive> prim)
{
  all_list_.add(prim);
}

void Owner::addTimer(boost::weak_ptr<timer::PeriodicTimer> timer)
{
  timer_list_.add(timer);
  all_list_.add(timer);
}

void Owner::addParameter(boost::weak_ptr<parameter::ParameterAbstract> parameter)
{
  parameter_list_.add(parameter);
  all_list_.add(parameter);
}

void Owner::latchStatistics(int32_t period_usec)
{
  for(PrimitiveList<Primitive>::PrimitiveListType::iterator it = all_list_.list_.begin();
      it != all_list_.list_.end();
      it++)
  {
    it->lock()->latchStatistics(period_usec);
  }
}


}
