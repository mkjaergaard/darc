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

namespace darc
{

void Owner::startPrimitives()
{
  for(PrimitiveListType::iterator it = list_.begin(); it != list_.end(); it++)
  {
    it->second.lock()->start();
  }
}

void Owner::stopPrimitives()
{
  for(PrimitiveListType::iterator it = list_.begin(); it != list_.end(); it++)
  {
    it->second.lock()->stop();
  }
}

void Owner::pausePrimitives()
{
  for(PrimitiveListType::iterator it = list_.begin(); it != list_.end(); it++)
  {
    it->second.lock()->pause();
  }
}

void Owner::unpausePrimitives()
{
  for(PrimitiveListType::iterator it = list_.begin(); it != list_.end(); it++)
  {
    it->second.lock()->unpause();
  }
}

void Owner::triggerPrimitivesOnAttach()
{
  for(PrimitiveListType::iterator it = list_.begin(); it != list_.end(); it++)
  {
    it->second.lock()->onAttach();
  }
}

void Owner::add(Primitive * item)
{
  list_.insert(PrimitiveListType::value_type(item->getID(), item->getWeakPtr()));

  // Trigger onAttach if the owner is already attached to a node
  if(isAttached())
  {
    item->onAttach();
  }
}

void Owner::latchStatistics(int32_t period_usec)
{
  for(PrimitiveListType::iterator it = list_.begin(); it != list_.end(); it++)
  {
    it->second.lock()->latchStatistics(period_usec);
  }
}

void Owner::printStatistics()
{
  for(PrimitiveListType::iterator it = list_.begin(); it != list_.end(); it++)
  {
    it->second.lock()->printStatistics();
  }
}

}
