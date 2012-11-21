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

#include <darc/distributed_container/shared_map_connection__decl.hpp>

namespace darc
{
namespace distributed_container
{

template<typename Key, typename T>
void connection<Key, T>::insert_data(const update_packet& update,
                                     buffer::shared_buffer data)
{
  for(size_t i = 0; i < update.num_entries_insert; i++)
  {
    inbound_data<serializer::boost_serializer, transfer_type> i_item(data);
    typename list_type::value_type value(i_item.get().first, i_item.get().second);
    list_.insert(value);
    parent_->insert_(remote_instance_id_, //informer
                     value.first, // key
                     value.second.first, // origin
                     value.second.second); // entry
  }

  for(size_t i = 0; i < update.num_entries_remove; i++)
  {
    inbound_data<serializer::boost_serializer, Key> i_item(data);
    list_.erase(i_item.get());
    parent_->remove_(remote_instance_id_, //informer
                     i_item.get()); // key
  }
}
}
}
