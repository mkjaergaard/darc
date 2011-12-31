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
 * DARC SharedBuffer class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_SHARED_BUFFER_H_INCLUDED__
#define __DARC_SHARED_BUFFER_H_INCLUDED__

#include <boost/shared_array.hpp>

class SharedBuffer : private boost::shared_array<uint8_t>
{
private:
  size_t size_;
  size_t start_offset_;

  SharedBuffer( size_t size ) :
    boost::shared_array<uint8_t>( new uint8_t[size] ),
    size_(size),
    start_offset_(0)
  {
  }

public:
  uint8_t * data() const
  {
    return boost::shared_array<uint8_t>::get() + start_offset_;
  }

  size_t size() const
  {
    return size_ - start_offset_;
  }

  void addOffset(size_t offset)
  {
    start_offset_ += offset;
  }

  static SharedBuffer create( size_t size )
  {
    return SharedBuffer( size );
  }

};

#endif
