/*
 * Copyright (c) 2012, Prevas A/S
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
 * DARC ParameterAbstract class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <darc/parameter/parameter_abstract.h>
#include <darc/parameter/parameter.h>
#include <darc/parameter/parameter_change.h>

namespace darc
{
namespace parameter
{

class Group
{
protected:
  typedef std::map<const std::string, ParameterAbstractWkPtr> ParameterListType;
  ParameterListType parameter_list_;

public:
  void registerParameter(ParameterAbstract * parameter)
  {
    assert(parameter_list_.count(parameter->getName()) == 0);
    parameter_list_[parameter->getName()] = parameter->getWeakPtr();
  }

  void unregisterParameter(ParameterAbstract * parameter)
  {
    // do something
  }

  void change(ParameterChange& change_info)
  {
    ParameterListType::iterator elem = parameter_list_.find(change_info.getName());
    assert(elem != parameter_list_.end());
    elem->second.lock()->change(change_info);
  }

  // dep?
  template<typename T>
  void setParameter(const std::string& name, const T& new_value)
  {
    ParameterListType::iterator elem = parameter_list_.find(name);
    assert(elem != parameter_list_.end());
    ParameterAbstractPtr param_shared = elem->second.lock();
    //todo: try!!
    boost::shared_ptr< Parameter<T> > t_param = boost::dynamic_pointer_cast<T>(param_shared);
    t_param->setValue(new_value);
  }

};

typedef boost::shared_ptr<Group> GroupPtr;

}
}
