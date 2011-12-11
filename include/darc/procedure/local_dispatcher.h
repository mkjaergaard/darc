#ifndef __DARC_PROCEDURE_LOCAL_DISPATCHER_H_INCLUDED__
#define __DARC_PROCEDURE_LOCAL_DISPATCHER_H_INCLUDED__

#include <vector>
#include <darc/procedure/client_impl.h>
#include <darc/procedure/server_impl.h>
#include <darc/procedure/local_dispatcher_abstract.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Ret, typename T_Sta>
class LocalDispatcher : public LocalDispatcherAbstract
{
private:
  std::string name_;
  
  typedef std::vector< typename ClientImpl<T_Arg, T_Ret, T_Sta>::Ptr > ClientListType; // <-- weak_ptr
  typedef std::vector< typename ServerImpl<T_Arg, T_Ret, T_Sta>::Ptr > ServerListType; // <-- weak_ptr

  ClientListType client_list_;
  ServerListType server_list_;

  void dispatchCall( boost::shared_ptr< T_Arg > )
  {
  }

  void dispatchStatus( boost::shared_ptr< T_Sta > )
  {
  }

  void dispatchReturn( boost::shared_ptr< T_Ret > )
  {
  }

  void registerClient( typename ClientImpl<T_Arg, T_Ret, T_Sta>::Ptr client )
  {
    client->registerDispatchFunction( boost::bind( &LocalDispatcher::dispatchReturn, this, _1 ),
				      boost::bind( &LocalDispatcher::dispatchStatus, this, _1 ) );
    client_list_.push_back( client );
  }

  void registerServer( typename ServerImpl<T_Arg, T_Ret, T_Sta>::Ptr server )
  {
    server_list_.push_back( server );
  }

};

}
}

#endif
