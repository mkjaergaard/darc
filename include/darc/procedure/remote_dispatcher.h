/*
 * Copyright (c) 2012, Prevas A/S
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
 * DARC Procedure RemoteDispatcher class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <map>
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <darc/serialization.h>
#include <darc/network/packet/header.h>
#include <darc/network/packet/procedure_call.h>
#include <darc/network/packet/procedure_advertise.h>
#include <darc/network/packet/procedure_result.h>
#include <darc/procedure/id_types.h>
#include <darc/log.h>

namespace darc
{

// fwd
namespace network
{
class LinkManager;
}

namespace procedure
{

class Manager;

class RemoteDispatcher
{
private:
  struct AdvertisedProcedureInfo
  {
    std::string procedure_name;
    darc::ID procedure_id;
    std::string argument_type_name;
    std::string feedback_type_name;
    std::string result_type_name;

    bool operator<(const AdvertisedProcedureInfo& other) const
    {
      return procedure_id < other.procedure_id;
    }

    AdvertisedProcedureInfo(const std::string& procedure_name, const darc::ID& procedure_id) :
      procedure_name(procedure_name),
      procedure_id(procedure_id)
    {
    }
  };

  typedef std::pair<darc::NodeID, AdvertisedProcedureInfo> RemoteAdvertisedProcedureInfo;

private:
  boost::asio::io_service * io_service_;
  Manager * manager_;
  network::LinkManager * network_link_manager_;

  // Procedures we advertise
  typedef std::set<AdvertisedProcedureInfo> AdvertisedProceduresType;
  AdvertisedProceduresType advertised_procedures_;

  // Procedures other nodes advertises
  typedef std::map<std::string, RemoteAdvertisedProcedureInfo> RemoteAdvertisedProceduresType;
  //  typedef std::pair<RemotePublishersType::iterator, RemotePublishersType::iterator> RemotePublishersRangeType;
  RemoteAdvertisedProceduresType remote_procedures_;

  // Signals
  //boost::signal<void (const std::string&, const std::string&, size_t)> signal_remote_subscriber_change_;
  //boost::signal<void (const std::string&, const std::string&, size_t)> signal_remote_publisher_change_;

public:
  RemoteDispatcher(boost::asio::io_service * io_service, Manager* manager, network::LinkManager * network_link_manager);

  /*
  boost::signal<void (const std::string&, const std::string&, size_t)>& remoteSubscriberChangeSignal()
  {
    return signal_remote_subscriber_change_;
  }

  boost::signal<void (const std::string&, const std::string&, size_t)>& remotePublisherChangeSignal()
  {
    return signal_remote_publisher_change_;
  }
  */

  void callReceiveHandler(const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len);
  void advertiseReceiveHandler( const network::packet::Header& header, SharedBuffer buffer, std::size_t data_len );

  void registerProcedure(const std::string& procedure_name,
			 const ID& procedure_id,
			 const std::string& argument_type_name,
			 const std::string& feedback_type_name,
			 const std::string& result_type_name);
  void sendAdvertisement(const AdvertisedProcedureInfo& info, const ID& remote_node_id);
  /*
  // Called by LocalDispatcher
  template<typename T>
  void postRemoteDispatch( const std::string& topic, const boost::shared_ptr<const T> msg )
  {
    if(remote_subscribers_.count(topic) != 0)
    {
      io_service_->post( boost::bind(&RemoteDispatcher::serializeAndDispatch<T>, this, topic, msg) );
    }
  }
  */
};

typedef boost::shared_ptr<RemoteDispatcher> RemoteDispatcherPtr;

}
}
