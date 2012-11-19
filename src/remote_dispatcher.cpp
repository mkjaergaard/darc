#include <darc/pubsub/remote_dispatcher.hpp>
#include <darc/pubsub/message_service.hpp>

namespace darc
{
namespace pubsub
{

void remote_dispatcher::handle_message_packet(const darc::ID& src_peer_id,
                                             darc::buffer::shared_buffer data)
{
  inbound_data<serializer::boost_serializer, message_packet> msg_i(data);

  // Just send the buffer to the dispatcher. not the nicest way
  parent_service_->remote_message_recv(msg_i.get().tag_id, data);
}

void remote_dispatcher::send_to(const ID& peer_id, const outbound_data_base& data)
{
  parent_service_->send_to(peer_id, data);
}

}
}
