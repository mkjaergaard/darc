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

#include <darc/ns/ns_service.hpp>

namespace darc
{

ns_service::ns_service(peer& p) :
  peer_service(p, 37)
{
  peer_connected_connection_ =
    p.peer_connected_signal().connect(
      boost::bind(&ns_service::peer_connected_handler, this, _1));
  peer_disconnected_connection_ =
    p.peer_disconnected_signal().connect(
      boost::bind(&ns_service::peer_disconnected_handler, this, _1));
}

void ns_service::peer_connected_handler(const ID& peer_id)
{
  for(local_tag_list_type::iterator it = local_tag_list_.begin();
      it != local_tag_list_.end();
      it++)
  {
    it->second->trigger_new_tag(it->second->id(), peer_id);
  }
}

void ns_service::peer_disconnected_handler(const ID& peer_id)
{
  for(local_tag_list_type::iterator it = local_tag_list_.begin();
      it != local_tag_list_.end();
      it++)
  {
    it->second->trigger_removed_tag(it->second->id(), peer_id);
  }
}

tag_handle ns_service::register_tag(const std::string& full_name)
{
  local_tag_ptr tag = local_tag::create(this, full_name);
  return boost::make_shared<tag_handle_impl>(boost::ref(tag));
}

void ns_service::attach_tag(local_tag_ptr tag)
{
  local_tag_list_.insert(local_tag_list_type::value_type(tag.get(), tag));
}

void ns_service::detach_tag(local_tag* tag)
{
  local_tag_list_.erase(tag);
}

}
