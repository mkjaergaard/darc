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

#include <string>
#include <boost/signals.hpp>
#include <boost/signals/connection.hpp>
#include <darc/id.hpp>
#include <boost/uuid/name_generator.hpp>

namespace darc
{
class ns_service;

class local_tag;
typedef boost::shared_ptr<local_tag> local_tag_ptr;

// manages lifetime of a local tag
class local_tag
{
protected:
  boost::uuids::name_generator gen_;
  ns_service * ns_service_;

  ID id_;
  std::string name_;

public:
  typedef void (callback_type)(const ID&, const ID&, const ID&);
  typedef boost::function<callback_type> functor_type;

protected:
  typedef boost::signal<callback_type> signal_type;
  signal_type new_tag_signal_;
  signal_type removed_tag_signal_;

public:
  local_tag(ns_service *,
            const std::string name);

  static local_tag_ptr create(ns_service *,
                              const std::string name);
  ~local_tag();

  void trigger_new_tag(const ID& tag_id, const ID& peer_id)
  {
    // Original tag ID, Remote tag ID, Remote Peer ID
    new_tag_signal_(id_, tag_id, peer_id);
  }

  void trigger_removed_tag(const ID& tag_id, const ID& peer_id)
  {
    removed_tag_signal_(id_, tag_id, peer_id);
  }

  signal_type& new_tag_signal()
  {
    return new_tag_signal_;
  }

  signal_type& removed_tag_signal()
  {
    return removed_tag_signal_;
  }

  const ID& id() const
  {
    return id_;
  }

  const std::string& name() const
  {
    return name_;
  }

};

}
