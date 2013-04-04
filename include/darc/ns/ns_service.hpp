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

#include <boost/signals/connection.hpp>
#include <darc/id.hpp>
#include <darc/peer/peer_service.hpp>
#include <darc/ns/local_tag.hpp>
#include <darc/ns/tag_handle.hpp>

namespace darc
{

class ns_service : public darc::peer_service
{
protected:
  boost::signals::connection peer_connected_connection_;
  boost::signals::connection peer_disconnected_connection_;

  typedef std::map<local_tag*, local_tag_ptr> local_tag_list_type;
  local_tag_list_type local_tag_list_;

protected:
  void peer_connected_handler(const ID& peer_id);
  void peer_disconnected_handler(const ID& peer_id);

  void recv(const darc::ID& src_peer_id,
            darc::buffer::shared_buffer data)
  {
  }

public:
  ns_service(peer& p);

  tag_handle register_tag(const std::string& full_name);
  void attach_tag(local_tag_ptr tag);
  void detach_tag(local_tag* tag);

};

}
