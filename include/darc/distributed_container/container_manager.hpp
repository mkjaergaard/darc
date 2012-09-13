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

#include <map>
#include <boost/function.hpp>
#include <darc/buffer/shared_buffer.hpp>
#include <darc/outbound_data.hpp>
#include <darc/id.hpp>

namespace darc
{
namespace distributed_container
{

class container_base; // fwd

class container_manager
{
public:
  typedef boost::function<void(const ID& destination, darc::buffer::shared_buffer)> send_to_function_type;
  send_to_function_type send_to_function_;

protected:
  typedef std::map<ID, container_base*> list_type;
  list_type list_;

  typedef std::map<ID, ID> instance_location_map_type;
  instance_location_map_type instance_location_map;

public:
  container_manager(send_to_function_type send_to_function);
  void attach(container_base* entry);
  void detatch(container_base* entry);
  void recv(const ID& src_location_id, darc::buffer::shared_buffer data);

  void send_to_instance(const ID& src_instance_id,
			const ID& dest_instance_id,
			const uint32_t payload_type,
			const outbound_data_base& data);

  void send_to_location(const ID& src_instance_id,
			const ID& dest_location_id,
			const ID& dest_instance_id,
			const uint32_t payload_type,
			const outbound_data_base& data);


};

}
}
