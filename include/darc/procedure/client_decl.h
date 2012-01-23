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
 * DARC Client class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PROCEDURE_CLIENT_H_INCLUDED__
#define __DARC_PROCEDURE_CLIENT_H_INCLUDED__

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <darc/procedure/local_dispatcher_fwd.h>
#include <darc/owner.h>
#include <darc/primitive.h>
#include <darc/enable_weak_from_static.h>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Ret, typename T_Sta>
class Client : public darc::Primitive, public::darc::EnableWeakFromStatic<Client<T_Arg, T_Ret, T_Sta> >
{
public:
  typedef boost::function<void( boost::shared_ptr<T_Ret>& )> ReturnHandlerType;
  typedef boost::function<void( boost::shared_ptr<T_Sta>& )> StatusHandlerType;

protected:
  boost::asio::io_service * io_service_;
  darc::Owner * owner_;
  std::string name_;
  ReturnHandlerType return_handler_;
  StatusHandlerType status_handler_;

  boost::weak_ptr<LocalDispatcher<T_Arg, T_Ret, T_Sta> > dispatcher_;

public:
  Client(darc::Owner* owner, const std::string& name, ReturnHandlerType return_handler, StatusHandlerType status_handler ) :
    io_service_(owner->getIOService()),
    owner_(owner),
    name_(name),
    return_handler_(return_handler),
    status_handler_(status_handler)
  {
    owner->addPrimitive(this->getWeakPtr());
  }

  // Called by darc::procedure::LocalDispatcher
  void postStatus( boost::shared_ptr<T_Sta>& msg )
  {
    io_service_->post( boost::bind( &Client::statusReceived, this, msg ) );
  }

  // Called by darc::procedure::LocalDispatcher
  void postReturn( boost::shared_ptr<T_Sta>& msg )
  {
    io_service_->post( boost::bind( &Client::returnReceived, this, msg ) );
  }

  void call( boost::shared_ptr<T_Arg>& argument );
  void onStart();
  void onStop();

protected:
  void statusReceived( boost::shared_ptr<T_Sta> msg )
  {
    if( status_handler_ )
    {
      status_handler_(msg);
    }
  }

  void returnReceived( boost::shared_ptr<T_Ret> msg )
  {
    if( return_handler_ )
    {
      return_handler_(msg);
    }
  }

};

}
}

#endif
