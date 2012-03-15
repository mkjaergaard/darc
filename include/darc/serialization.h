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
 * DARC SerializedMessage class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_SERIALIZED_MESSAGE_H_INCLUDED_
#define __DARC_SERIALIZED_MESSAGE_H_INCLUDED_

#include <boost/shared_ptr.hpp>
#include <ros/serialization.h>
#include <ros/message_traits.h>
#include <darc/shared_buffer.h>
#include <darc/network/packet/parser.h>

namespace darc
{

class Serialization
{

public:
  template<typename T>
  static boost::shared_ptr<const T> deserialize(SharedBuffer msg_data)
  {
    boost::shared_ptr<T> msg( new T() );

    // This should optimally be handled by the msg serializer
    // Read Type Info
    std::string type_name;
    uint64_t md5_value1 = 0;
    uint64_t md5_value2 = 0;
    msg_data.addOffset( network::packet::Parser::readString( type_name, msg_data.data(), msg_data.size() ) );
    // MD5
    msg_data.addOffset( network::packet::Parser::readUint64( md5_value1, msg_data.data(), msg_data.size() ) );
    msg_data.addOffset( network::packet::Parser::readUint64( md5_value2, msg_data.data(), msg_data.size() ) );

    // Some Debug Stuff
    if( type_name != std::string(ros::message_traits::DataType<T>::value()) ||
        md5_value1 != ros::message_traits::MD5Sum<T>::static_value1 ||
        md5_value2 != ros::message_traits::MD5Sum<T>::static_value2 )
    {
      std::cout << type_name << ";" << std::hex << md5_value1 << ";" << md5_value2 << std::endl;
      assert(false);
    }

    // Read actual message
    ros::serialization::IStream in( msg_data.data(), msg_data.size() );
    ros::serialization::deserialize(in, *(msg.get()) );
    return msg;
  }

  template<typename T>
  static void serialize(SharedBuffer buffer, boost::shared_ptr<const T> msg)
  {
    // Write Type Info
    buffer.addOffset( network::packet::Parser::writeString(ros::message_traits::DataType<T>::value(), buffer.data(), buffer.size()) );
    // MD5
    buffer.addOffset( network::packet::Parser::writeUint64(ros::message_traits::MD5Sum<T>::static_value1, buffer.data(), buffer.size()) );
    buffer.addOffset( network::packet::Parser::writeUint64(ros::message_traits::MD5Sum<T>::static_value2, buffer.data(), buffer.size()) );

    // Serialize actual message
    ros::serialization::OStream ostream( buffer.data(), buffer.size() );
    ros::serialization::serialize( ostream, *(msg.get()) );
  }

  template<typename T>
  static std::size_t size(boost::shared_ptr<const T> msg)
  {
    return ros::serialization::serializationLength(*msg) +
      + 16/*MD5*/
      + strlen(ros::message_traits::DataType<T>::value()) + 1; // +1 is the null-term
  }
};

}

#endif
