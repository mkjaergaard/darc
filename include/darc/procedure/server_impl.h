#ifndef __DARC_PROCEDURE_SERVER_IMPL_H_INCLUDED__
#define __DARC_PROCEDURE_SERVER_IMPL_H_INCLUDED__

#include <boost/shared_ptr.hpp>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Ret, typename T_Sta>
class ServerImpl
{
public:
  typedef boost::shared_ptr< ServerImpl<T_Arg, T_Ret, T_Sta> > Ptr;
  typedef boost::function<void( boost::shared_ptr<T_Arg> )> MethodType;

  typedef boost::function< void( boost::shared_ptr<T_Ret> ) > DispatchReturnFunctionType;
  typedef boost::function< void( boost::shared_ptr<T_Sta> ) > DispatchStatusFunctionType;

protected:
  boost::asio::io_service * io_service_;
  MethodType method_;

  DispatchReturnFunctionType dispatch_return_function_;
  DispatchStatusFunctionType dispatch_status_function_;

public:
  ServerImpl( boost::asio::io_service * io_service, const std::string& name, MethodType method ) :
    io_service_(io_service),
    method_(method)
  {
  }

  // Called by darc::procedure::Dispatcher
  void registerDispatchFunctions( DispatchReturnFunctionType dispatch_return_function, DispatchStatusFunctionType dispatch_status_function )
  {
    dispatch_return_function_ = dispatch_return_function;
    dispatch_status_function_ = dispatch_status_function;
  }

};

}
}

#endif
