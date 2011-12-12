#ifndef __DARC_PROCEDURE_CLIENT_H_INCLUDED__
#define __DARC_PROCEDURE_CLIENT_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <darc/procedure/client_impl.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Ret, typename T_Sta>
class Client
{
private:
  typename ClientImpl<T_Arg, T_Ret, T_Sta>::Ptr impl_;

public:
  Client(darc::Component * owner,
	 const std::string& name,
	 typename ClientImpl<T_Arg, T_Ret, T_Sta>::ReturnHandlerType return_handler,
	 typename ClientImpl<T_Arg, T_Ret, T_Sta>::StatusHandlerType status_handler) :
    impl_( new ClientImpl<T_Arg, T_Ret, T_Sta>(owner->getIOService(), name, return_handler, status_handler) )
  {
    owner->getNode()->getProcedureManager().registerClient<T_Arg, T_Ret, T_Sta>(name, impl_);
  }

  ~Client()
  {
    //unregister
  }

  void call(boost::shared_ptr<T_Arg> argument)
  {
    impl_->call(argument);
  }

};

}
}

#endif
