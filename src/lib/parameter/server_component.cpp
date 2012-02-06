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
 * DARC Parameter Component Server
 *
 * \author Morten Kjaergaard
 */

#include <darc/darc.h>
#include <darc/timer/periodic_timer.h>
#include <darc/parameter/component_parameter_source.h>

namespace darc
{
namespace parameter
{

class ParameterServerComponent : public darc::Component
{
protected:
  typedef std::map<const std::string, ComponentParameterSource*> ComponentParameterSourceListType;
  ComponentParameterSourceListType source_list_;

  darc::timer::PeriodicTimer timer_;

protected:
  void timerHandler()
  {
    for( ComponentParameterSourceListType::iterator it = source_list_.begin(); it != source_list_.end(); it++ )
    {
      if( (*it).second->isLinked() == false )
      {
	ID id = node_->lookupComponentInstance( (*it).second->getComponentInstanceName() );
	if( id != nullID() )
	{
	  DARC_INFO("Found Component %s", (*it).second->getComponentInstanceName().c_str() );
	  (*it).second->linkComponent(id);
	}
      }
    }
  }

public:
  ParameterServerComponent(const std::string& name, boost::shared_ptr<Node> node) :
    darc::Component(name, node),
    timer_( this, boost::bind(&ParameterServerComponent::timerHandler, this), boost::posix_time::seconds(1) )
  {
    ComponentParameterSource * s = new ComponentParameterSource(this, "MyPublisherComponent");
    source_list_["MyPublisherComponent"] = s;
  }

};

DARC_REGISTER_COMPONENT(ParameterServerComponent);

}
}
