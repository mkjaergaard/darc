#ifndef __DARC_PROCEDURE_LOCAL_DISPATCHER_MANAGER_INCLUDED__
#define __DARC_PROCEDURE_LOCAL_DISPATCHER_MANAGER_INCLUDED__

#include <map>
#include <boost/shared_ptr.hpp>
#include <darc/procedure/local_dispatcher.h>
#include <darc/procedure/client_impl.h>
#include <darc/procedure/server_impl.h>

namespace darc
{
namespace procedure
{

class LocalDispatcherManager
{
public:
  typedef boost::shared_ptr<LocalDispatcherManager> Ptr;

private:
  typedef std::map< const std::string, LocalDispatcherAbstract::Ptr > DispatcherListType;
  DispatcherListType dispatcher_list_;

public:
  template<typename T_Arg, typename T_Ret, typename T_Sta>
  void registerClient( typename ClientImpl<T_Arg, T_Ret, T_Sta>::Ptr )
  {
    
  }

  template<typename T_Arg, typename T_Ret, typename T_Sta>
  void registerServer( typename ServerImpl<T_Arg, T_Ret, T_Sta>::Ptr )
  {
    
  }

private:
  template<typename T_Arg, typename T_Ret, typename T_Sta>
  typename LocalDispatcher<T_Arg, T_Ret, T_Sta>::Ptr getLocalDispatcher( const std::string& name )
  {                                                             
    DispatcherListType::iterator elem = dispatcher_list_.find(name);
    if( elem == dispatcher_list_.end() )
    {
      typename LocalDispatcher<T_Arg, T_Ret, T_Sta>::Ptr dispatcher( new LocalDispatcher<T_Arg, T_Ret, T_Sta>() );
      dispatcher_list_[ name ] = dispatcher;
      return dispatcher;
    }
    else
    {
      LocalDispatcherAbstract::Ptr &dispatcher_a = elem->second;
      // todo, try
      typename LocalDispatcher<T_Arg, T_Ret, T_Sta>::Ptr dispatcher = boost::dynamic_pointer_cast<LocalDispatcher<T_Arg, T_Ret, T_Sta> >(dispatcher_a);
      return dispatcher;
    }
  }

};

}
}

#endif
