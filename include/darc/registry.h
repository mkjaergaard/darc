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
 * DARC Registry class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <map>
#include <boost/function.hpp>
#include <darc/component_fwd.h>
#include <darc/node.h>

namespace darc
{

class Registry
{
private:
  typedef boost::function<ComponentPtr(const std::string&, NodePtr)> InstantiateComponentMethod;
  typedef std::map<const std::string, InstantiateComponentMethod> ComponentListType;

  ComponentListType component_list_;

  static Registry * instance_;

private:
  Registry();
  static Registry * instance();

public:
  static int registerComponent(const std::string& component_name, InstantiateComponentMethod method);
  static darc::ComponentPtr instantiateComponent(const std::string& instance_name, NodePtr node);

};

}

#define DARC_REGISTER_COMPONENT(classname) \
namespace classname##_reg\
{\
static int dummy = darc::Registry::registerComponent( #classname, boost::bind(&classname::instantiate<classname>, _1, _2) ); \
}
