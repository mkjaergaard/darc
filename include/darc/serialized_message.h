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
#include <darc/packet/parser.h>

namespace darc
{

class SerializedMessage
{
public:
  typedef boost::shared_ptr<SerializedMessage> Ptr;
  typedef const boost::shared_ptr<const SerializedMessage> ConstPtr;

  SharedBuffer buffer_;
  size_t data_size_;

public:
  SerializedMessage() :
    buffer_( SharedBuffer::create(0) ),
    data_size_(0)
  {
  }

  SerializedMessage( SharedBuffer buffer, size_t data_size ) :
    buffer_(buffer),
    data_size_(data_size)
  {
  }

  template<typename T>
  explicit SerializedMessage( const boost::shared_ptr<const T> &msg ) :
    buffer_( SharedBuffer::create(0) ),
    data_size_(0)
  {
    serializeMsg<T>(msg);
  }

  template<typename T>
  uint32_t serializeMsg( const boost::shared_ptr<const T> msg)
  {
    // Allocate proper buffer size
    data_size_ = 1024;//ros::serialization::Serializer<T>::serializedLength(*(msg.get()));
    buffer_ = SharedBuffer::create( data_size_ );

    // This should optimally be handled by the msg serializer
    // Write Type Info
    size_t pos = packet::Parser::writeString( ros::message_traits::DataType<T>::value(), buffer_.data(), buffer_.size() );
    // MD5
    pos += packet::Parser::writeUint64( ros::message_traits::MD5Sum<T>::static_value1, buffer_.data() + pos, buffer_.size() - pos );
    pos += packet::Parser::writeUint64( ros::message_traits::MD5Sum<T>::static_value2, buffer_.data() + pos, buffer_.size() - pos );

    // Serialize actual message
    ros::serialization::OStream ostream( buffer_.data() + pos, buffer_.size() - pos );
    ros::serialization::serialize( ostream, *(msg.get()) );
    return data_size_;
  }

  template<typename T>
  boost::shared_ptr<T> deserialize() const
  {
    boost::shared_ptr<T> msg( new T() );

    // This should optimally be handled by the msg serializer
    // Read Type Info
    std::string type_name;
    uint64_t md5_value1 = 0;
    uint64_t md5_value2 = 0;
    size_t pos = packet::Parser::readString( type_name, buffer_.data(), buffer_.size() );
    // MD5
    pos += packet::Parser::readUint64( md5_value1, buffer_.data() + pos, buffer_.size() - pos );
    pos += packet::Parser::readUint64( md5_value2, buffer_.data() + pos, buffer_.size() - pos );

    // Some Debug Stuff
    if( type_name != std::string(ros::message_traits::DataType<T>::value()) ||
        md5_value1 != ros::message_traits::MD5Sum<T>::static_value1 ||
        md5_value2 != ros::message_traits::MD5Sum<T>::static_value2 )
    {
      std::cout << type_name << ";" << std::hex << md5_value1 << ";" << md5_value2 << std::endl;
      assert(false);
    }

    // Read actual message
    ros::serialization::IStream in( buffer_.data() + pos, buffer_.size() - pos );
    ros::serialization::deserialize(in, *(msg.get()) );
    return msg;
  }

  const SharedBuffer& getBuffer() const
  {
    return buffer_;
  }

};

}

#endif
