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
 * DARC SharedBufferZmq class
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <zmq.hpp>
#include <boost/shared_ptr.hpp>
#include <darc/shared_buffer.h>

namespace darc
{
namespace network
{
namespace zeromq
{

class SharedBufferZmq : public SharedBufferImpl
{
private:
  zmq::message_t message_;

private:
  SharedBufferZmq() :
    SharedBufferImpl()
  {
  }

public:
  zmq::message_t * message()
  {
    return &message_;
  }

  uint8_t * _data()
  {
    return (uint8_t*)message_.data() + start_offset_;
  }

  size_t _size()
  {
    return message_.size() - start_offset_;
  }

  static boost::shared_ptr<SharedBufferZmq> create()
  {
    return boost::shared_ptr<SharedBufferZmq>(new SharedBufferZmq());
  }

};

typedef boost::shared_ptr<SharedBufferZmq> SharedBufferZmqPtr;

}
}
}
