#include <darc/peer.hpp>
#include <darc/peer_service.hpp>

namespace darc
{

void peer::recv(const ID& src_peer_id, buffer::shared_buffer data)
{
  inbound_data<darc::serializer::boost_serializer, service_header_packet> header_i(data);

  service_list_type::iterator item = service_list_.find(header_i.get().service_type);
  if(item != service_list_.end())
  {
    item->second->recv(src_peer_id, data);
  }
  else
  {
    iris::glog<iris::Warning>("Received Data for unknown service id",
                              "service", iris::arg<uint32_t>(header_i.get().service_type));
  }
}

}
