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
 *
 *
 * \author Morten Kjaergaard
 */

#pragma once

#include <boost/utility.hpp>
#include <darc/buffer/shared_buffer.hpp>

namespace darc
{

class outbound_data_base
{
public:
  virtual ~outbound_data_base()
  {
  }

  virtual void pack(buffer::shared_buffer& buffer) const = 0;
};

template<typename S, typename T>
class outbound_data : public outbound_data_base, boost::noncopyable
{
protected:
  const T& data_;

public:
  outbound_data(const T& data) :
    data_(data)
  {
  }

  virtual void pack(buffer::shared_buffer& buffer) const
  {
    S::template pack<T>(buffer, data_);
  }
};

class outbound_pair : public outbound_data_base
{
protected:
  const outbound_data_base& first_;
  const outbound_data_base& second_;

public:
  outbound_pair(const outbound_data_base& first, const outbound_data_base& second) :
    first_(first),
    second_(second)
  {
  }

  virtual void pack(buffer::shared_buffer& buffer) const
  {
    first_.pack(buffer);
    second_.pack(buffer);
  }

};

}
