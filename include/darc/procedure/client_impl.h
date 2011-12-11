#ifndef __DARC_PROCEDURE_CLIENT_IMPL_H_INCLUDED__
#define __DARC_PROCEDURE_CLIENT_IMPL_H_INCLUDED__

#include <boost/shared_ptr.hpp>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Ret, typename T_Sta>
class ClientImpl
{
public:
  typedef boost::shared_ptr< ClientImpl<T_Arg, T_Ret, T_Sta> > Ptr;

  typedef boost::function<void( boost::shared_ptr<T_Ret> )> ReturnHandlerType;
  typedef boost::function<void( boost::shared_ptr<T_Sta> )> StatusHandlerType;

  typedef boost::function< void( boost::shared_ptr<T_Arg> ) > DispatchCallFunctionType;

protected:
  boost::asio::io_service * io_service_;
  ReturnHandlerType return_handler_;

  DispatchCallFunctionType dispatch_call_function_;

public:
  ClientImpl( boost::asio::io_service * io_service, const std::string& name, ReturnHandlerType return_handler ) :
    io_service_(io_service),
    return_handler_(return_handler)
  {
  }

  // Called by darc::procedure::LocalDispatcher
  void registerDispatchFunctions( DispatchCallFunctionType dispatch_call_function )
  {
    dispatch_call_function_ = dispatch_call_function;
  }

  void call(boost::shared_ptr<T_Arg> argument)
  {
    dispatch_call_function_(argument);
  }

};

}
}

#endif
