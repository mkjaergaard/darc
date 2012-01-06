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
 * DARC RemoteDispatcherManager class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PUBLISH_REMOTE_DISPATCH_HANDLER_H_INCLUDED__
#define __DARC_PUBLISH_REMOTE_DISPATCH_HANDLER_H_INCLUDED__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <darc/serialization.h>
#include <darc/packet/header.h>
#include <darc/packet/message.h>

namespace darc
{
namespace pubsub
{

class RemoteDispatcher
{
private:
  boost::asio::io_service * io_service_;

  // Function to dispatch locally
  typedef boost::function<void (const std::string& topic, SharedBuffer)> LocalDispatchFunctionType;
  LocalDispatchFunctionType local_dispatch_function_;

  // Function to send to remote node
  typedef boost::function<void (packet::Header::PayloadType, SharedBuffer, std::size_t )> SendToNodeFunctionType;
  SendToNodeFunctionType send_to_node_function_;

public:
  RemoteDispatcher( boost::asio::io_service * io_service ) :
    io_service_( io_service )
  {
  }

  void packetReceiveHandler( SharedBuffer buffer, std::size_t data_len )
  {
    // Parse Msg Packet
    packet::Message msg_packet;
    size_t msg_header_size = msg_packet.read( buffer.data(), data_len );
    buffer.addOffset( msg_header_size );

    // Dispatch to local subscribers
    local_dispatch_function_( msg_packet.topic, buffer );
  }

  void setLocalDispatchFunction( LocalDispatchFunctionType local_dispatch_function )
  {
    local_dispatch_function_ = local_dispatch_function;
  }

  void setSendToNodeFunction( SendToNodeFunctionType send_to_node_function )
  {
    send_to_node_function_ = send_to_node_function;
  }

  // Triggered by asio post
  template<typename T>
  void serializeAndDispatch( const std::string topic, const boost::shared_ptr<const T> msg )
  {
    // Allocate buffer. todo: derive required size from msg
    std::size_t data_len = 1024;
    SharedBuffer buffer = SharedBuffer::create(data_len);

    // Message Header
    packet::Message msg_header(topic);
    std::size_t pos = msg_header.write( buffer.data(), buffer.size() );

    // todo: Put common serialization stuff somewhere to reuse
    // Write Type Info
    pos += packet::Parser::writeString( ros::message_traits::DataType<T>::value(), buffer.data() + pos, buffer.size() - pos );
    // MD5
    pos += packet::Parser::writeUint64( ros::message_traits::MD5Sum<T>::static_value1, buffer.data() + pos, buffer.size() - pos );
    pos += packet::Parser::writeUint64( ros::message_traits::MD5Sum<T>::static_value2, buffer.data() + pos, buffer.size() - pos );

    // Serialize actual message
    ros::serialization::OStream ostream( buffer.data() + pos, buffer.size() - pos );
    ros::serialization::serialize( ostream, *(msg.get()) );

    assert( send_to_node_function_ );
    send_to_node_function_( packet::Header::MSG_PACKET, buffer, data_len );
  }

  // Called by LocalDispatcher
  template<typename T>
  void postRemoteDispatch( const std::string& topic, const boost::shared_ptr<const T> msg )
  {
    // if( remote subscribers )
    io_service_->post( boost::bind(&RemoteDispatcher::serializeAndDispatch<T>, this, topic, msg) );
  }

};

typedef boost::shared_ptr<RemoteDispatcher> RemoteDispatcherPtr;

}
}

#endif
