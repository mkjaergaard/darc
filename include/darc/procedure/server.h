#ifndef __DARC_PROCEDURE_SERVER_H_INCLUDED__
#define __DARC_PROCEDURE_SERVER_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <darc/procedure/server_impl.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Ret, typename T_Sta>
class Server
{
private:
  typename ServerImpl<T_Arg, T_Ret, T_Sta>::Ptr impl_;

public:
  Server(darc::Component * owner, const std::string& name, typename ServerImpl<T_Arg, T_Ret, T_Sta>::MethodType method) :
    impl_( new ServerImpl<T_Arg, T_Ret, T_Sta>(owner->getIOService(), name, method) )
  {
    owner->getNode()->getProcedureManager().registerServer(name, impl_);
  }

  ~Server()
  {
    //unregister
  }

};

}
}

#endif
