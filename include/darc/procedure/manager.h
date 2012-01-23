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
 * DARC Manager class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PROCEDURE_MANAGER_INCLUDED__
#define __DARC_PROCEDURE_MANAGER_INCLUDED__

#include <map>
#include <boost/shared_ptr.hpp>
#include <darc/procedure/local_dispatcher.h>

namespace darc
{
namespace procedure
{

class Manager
{
public:
  typedef boost::shared_ptr<Manager> Ptr;

private:
  typedef std::map< const std::string, LocalDispatcherAbstract::Ptr > DispatcherListType;
  DispatcherListType dispatcher_list_;

public:
  template<typename T_Arg, typename T_Ret, typename T_Sta>
  boost::shared_ptr<LocalDispatcher<T_Arg, T_Ret, T_Sta> > getLocalDispatcher( const std::string& name )
  {
    DispatcherListType::iterator elem = dispatcher_list_.find(name);
    if( elem == dispatcher_list_.end() )
    {
      boost::shared_ptr<LocalDispatcher<T_Arg, T_Ret, T_Sta> > dispatcher( new LocalDispatcher<T_Arg, T_Ret, T_Sta>() );
      dispatcher_list_[ name ] = dispatcher;
      return dispatcher;
    }
    else
    {
      LocalDispatcherAbstract::Ptr &dispatcher_a = elem->second;
      // todo, try
      boost::shared_ptr<LocalDispatcher<T_Arg, T_Ret, T_Sta> > dispatcher = boost::dynamic_pointer_cast<LocalDispatcher<T_Arg, T_Ret, T_Sta> >(dispatcher_a);
      return dispatcher;
    }
  }

};

}
}

#endif
