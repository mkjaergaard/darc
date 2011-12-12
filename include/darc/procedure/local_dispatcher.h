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
public:
  typedef boost::shared_ptr< LocalDispatcher<T_Arg, T_Ret, T_Sta> > Ptr;

private:
  std::string name_;
  
  typedef std::vector< typename ClientImpl<T_Arg, T_Ret, T_Sta>::Ptr > ClientListType; // <-- weak_ptr

  typename ServerImpl<T_Arg, T_Ret, T_Sta>::Ptr server_;
  ClientListType client_list_;

public:
  void dispatchCall( boost::shared_ptr< T_Arg > arg )
  {
    dispatchCallLocally(arg);
  }

  void dispatchStatus( boost::shared_ptr< T_Sta > msg )
  {
    dispatchStatusLocally(msg);
  }

  void dispatchReturn( boost::shared_ptr< T_Ret > msg )
  {
    dispatchReturnLocally(msg);
  }

  void registerClient( typename ClientImpl<T_Arg, T_Ret, T_Sta>::Ptr client )
  {
    client->registerDispatchFunctions( boost::bind( &LocalDispatcher::dispatchCall, this, _1 ) );
    client_list_.push_back( client );
  }

  void registerServer( typename ServerImpl<T_Arg, T_Ret, T_Sta>::Ptr server )
  {
    assert(server_.get() == 0);
    server->registerDispatchFunctions( boost::bind( &LocalDispatcher::dispatchReturn, this, _1 ),
				       boost::bind( &LocalDispatcher::dispatchStatus, this, _1 ) );
    server_ = server;
  }

private:
  void dispatchCallLocally( boost::shared_ptr<T_Sta>& arg )
  {
    if( server_.get() != 0 )
    {
      server_->postCall(arg);
    }
  }

  void dispatchStatusLocally( boost::shared_ptr<T_Sta>& msg )
  {
    // todo: only dispatch to the actual caller
    for( typename ClientListType::iterator it = client_list_.begin();
	 it != client_list_.end();
	 it++)
    {
      (*it)->postStatus(msg);
    }
  }

  void dispatchReturnLocally( boost::shared_ptr<T_Sta>& msg )
  {
    // todo: only dispatch to the actual caller
    for( typename ClientListType::iterator it = client_list_.begin();
	 it != client_list_.end();
	 it++)
    {
      (*it)->postReturn(msg);
    }    
  }

};

}
}

#endif
