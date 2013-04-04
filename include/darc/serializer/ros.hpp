/*
 * Copyright (c) 2013, Prevas A/S
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
 *
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <ros/serialization.h>
#include <darc/buffer/shared_buffer.hpp>
#include <iris/glog.hpp>

namespace darc
{
namespace serializer
{

struct ros_serializer
{
  template<typename T>
  static void pack(buffer::shared_buffer& buffer, const T& data)
  {
    ros::serialization::OStream out((uint8_t*)buffer->pptr(), 1024*5); //todo: get correct size
    uint32_t initial_len = out.getLength();
    ros::serialization::serialize(out, data);
    buffer->streambuf()->pubseekoff(out.getLength() - initial_len, std::ios_base::cur);
  }

  template<typename T>
  static void unpack(buffer::shared_buffer& buffer, T& data)
  {
    ros::serialization::IStream in((uint8_t*)buffer->gptr(), 1024*5); //todo: get correct size
    uint32_t initial_len = in.getLength();
    ros::serialization::deserialize(in, data);
    buffer->streambuf()->pubseekoff(initial_len - in.getLength(), std::ios_base::cur);
  }
};

}
}
