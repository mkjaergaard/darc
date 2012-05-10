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

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class SharedBufferImpl
{
protected:
  size_t start_offset_;

  SharedBufferImpl(size_t s = 0) :
    start_offset_(0)
  {
  }

  virtual ~SharedBufferImpl()
  {
  }
  
  virtual uint8_t * _data() = 0;
  virtual size_t _size() = 0;

public:
  virtual uint8_t * data()
  {
    return _data() + start_offset_;
  }

  virtual size_t size()
  {
    return _size() - start_offset_;
  }  

  virtual void addOffset(size_t offset)
  {
    start_offset_ += offset;
  }

  virtual void resetOffset()
  {
    start_offset_ = 0;
  }

};

class SharedBuffer
{
public:
  typedef boost::shared_ptr<SharedBufferImpl> ImplType;

protected:
  ImplType impl_;

public:
  SharedBuffer(ImplType impl) :
    impl_(impl)
  {
  }

  uint8_t * data() const
  {
    return impl_->data();
  }

  size_t size() const
  {
    return impl_->size();
  }

  void addOffset(size_t offset)
  {
    return impl_->addOffset(offset);
  }

  void resetOffset()
  {
    return impl_->resetOffset();
  }

};

#endif
