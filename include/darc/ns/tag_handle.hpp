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
 * \author Morten Kjaergaard
 */

#pragma once

#include <darc/id.hpp>
#include <darc/ns/local_tag.hpp>

namespace darc
{

class tag_handle_impl
{
public:
  typedef local_tag::functor_type listener_type;

  boost::signals::connection new_tag_signal_connection_;
  boost::signals::connection removed_tag_signal_connection_;

protected:
  local_tag_ptr local_tag_;

public:
  tag_handle_impl(local_tag_ptr& local_tag) :
    local_tag_(local_tag)
  {
  }

  ~tag_handle_impl()
  {
    new_tag_signal_connection_.disconnect();
    removed_tag_signal_connection_.disconnect();
  }

  const ID& id() const
  {
    return local_tag_->id();
  }

  const std::string& name() const
  {
    return local_tag_->name();
  }

  void connect_new_tag_listener(local_tag::functor_type callback)
  {
    new_tag_signal_connection_ = local_tag_->new_tag_signal().connect(callback);
  }

  void connect_removed_tag_listener(local_tag::functor_type callback)
  {
    removed_tag_signal_connection_ = local_tag_->removed_tag_signal().connect(callback);
  }

  void reset()
  {
    new_tag_signal_connection_.disconnect();
    removed_tag_signal_connection_.disconnect();
    local_tag_.reset();
  }

};

typedef boost::shared_ptr<tag_handle_impl> tag_handle;

}
