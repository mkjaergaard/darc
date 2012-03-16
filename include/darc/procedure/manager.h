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
 * DARC Manager (Template Definitions)
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PROCEDURE_MANAGER_INCLUDED__
#define __DARC_PROCEDURE_MANAGER_INCLUDED__

#include <darc/procedure/manager_decl.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Result, typename T_Feedback>
boost::shared_ptr<LocalDispatcher<T_Arg, T_Result, T_Feedback> > Manager::getLocalDispatcher(const std::string& name)
{
  ProcedureIDListType::iterator elem = procedure_id_list_.find(name);
  if( elem == procedure_id_list_.end() )
  {
    boost::shared_ptr<LocalDispatcher<T_Arg, T_Result, T_Feedback> >
      dispatcher(new LocalDispatcher<T_Arg, T_Result, T_Feedback>(name, this));
    procedure_id_list_[name] = dispatcher->getProcedureID();
    dispatcher_list_[dispatcher->getProcedureID()] = dispatcher;
    return dispatcher;
  }
  else
  {
    DispatcherListType::iterator dispatcher_elem = dispatcher_list_.find(elem->second);
    if(dispatcher_elem != dispatcher_list_.end())
    {
      LocalDispatcherAbstractPtr &dispatcher_a = dispatcher_elem->second;
      // todo, try
      boost::shared_ptr<LocalDispatcher<T_Arg, T_Result, T_Feedback> > dispatcher =
	boost::dynamic_pointer_cast<LocalDispatcher<T_Arg, T_Result, T_Feedback> >(dispatcher_a);
      return dispatcher;
    }
    else
    {
      DARC_FATAL("This is not right");
    }
  }
}

}
}

#endif
