#pragma once

#include <darc/peer_service.hpp>

#include <darc/inbound_data.hpp>
#include <darc/serializer/boost.hpp>

#include <darc/message_packet.hpp>
#include <darc/subscribe_packet.hpp>

namespace darc
{

class RemoteDispatcher : public darc::peer_service
{
protected:


protected:
  void recv(const darc::ID& src_peer_id,
	    darc::service_type service_id,
	    darc::buffer::shared_buffer data)
  {
    darc::inbound_data<darc::serializer::boost_serializer, uint32_t> payload_type_i(data);
    switch(payload_type_i.get())
    {
    case subscribe_packet::payload_id:
    {
      handle_subscribe_packet(src_peer_id, data);
    }
    break;
    case message_packet::payload_id:
    {
      handle_message_packet(src_peer_id, data);
    }
    break;
    default:
      assert(0);
    }
  }

  void handle_subscribe_packet(const darc::ID& src_peer_id,
			       darc::buffer::shared_buffer data)
  {
  }

  void handle_message_packet(const darc::ID& src_peer_id,
			     darc::buffer::shared_buffer data)
  {
  }

};

}
