#include <darc/network/inbound_link_base.hpp>

#include <darc/network/network_manager.hpp>

namespace darc
{
namespace network
{

void inbound_link_base::handle_discover_reply_packet(const ID& src_peer_id, buffer::shared_buffer& data)
{
  inbound_data<darc::serializer::boost_serializer, discover_reply_packet> drp_i(data);

  beam::glog<beam::Info>("Received DISCOVER_REPLY",
                         "peer_id", beam::arg<ID>(src_peer_id),
                         "outbound_id", beam::arg<ID>(drp_i.get().outbound_id));

  manager_->discover_reply_received(src_peer_id, drp_i.get().outbound_id);

}

void inbound_link_base::packet_received(buffer::shared_buffer header_data,
                                        buffer::shared_buffer body_data)
{
  inbound_data<darc::serializer::boost_serializer, link_header_packet> header_i(header_data);

  // Discard packages not to us, or from self, e.g. due to multicasting
  if((header_i.get().dest_peer_id != ID::null() &&
      header_i.get().dest_peer_id != peer_.id()) ||
     header_i.get().src_peer_id == peer_.id())
  {
    return;
  }

  // Switch on packet type
  switch(header_i.get().packet_type)
  {
  case link_header_packet::SERVICE:
  {
    manager_->service_packet_received(header_i.get().src_peer_id, body_data);
    break;
  }
  case link_header_packet::DISCOVER:
  {
    handle_discover_packet(header_i.get().src_peer_id, body_data);
    break;
  }
  case link_header_packet::DISCOVER_REPLY:
  {
    handle_discover_reply_packet(header_i.get().src_peer_id, body_data);
    break;
  }
  default:
  {
    beam::glog<beam::Warning>("NetworkManager: Unknown packet type",
                              "type", beam::arg<int>(header_i.get().packet_type));
    break;
  }
  }
}

}
}
