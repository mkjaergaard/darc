#include <darc/pubsub/message_service.hpp>

namespace darc
{
namespace pubsub
{

message_service::message_service(peer& p, boost::asio::io_service& io_service, ns_service& ns_service) :
  peer_service(p, 13),
  io_service_(io_service),
  nameserver_(ns_service)
{
  peer_connected_connection_ =
      p.peer_connected_signal().connect(
          boost::bind(&message_service::peer_connected_handler, this, _1));
  peer_disconnected_connection_ =
      p.peer_disconnected_signal().connect(
          boost::bind(&message_service::peer_disconnected_handler, this, _1));
}

/**
 * Network packages are passed on to remote_dispatcher.
 */
void message_service::recv(const darc::ID& src_peer_id,
                           darc::buffer::shared_buffer data)
{
  darc::inbound_data<darc::serializer::boost_serializer,
                     payload_header_packet> payload_type_i(data);
  switch(payload_type_i.get().payload_type)
  {
  case subscribe_packet::payload_id:
  {
    //handle_subscribe_packet(src_peer_id, data);
  }
  break;
  case publish_packet::payload_id:
  {
    //handle_publish_packet(src_peer_id, data);
  }
  break;
  case message_packet::payload_id:
  {
    inbound_data<serializer::boost_serializer, message_packet> msg_i(data);

    // Just send the buffer to the dispatcher. not the nicest way
    remote_message_recv(msg_i.get().tag_id, data);
  }
  break;
  default:
    iris::glog<iris::Fatal>
      ("Unknown payload",
       "payload_id:", iris::arg<int>(payload_type_i.get().payload_type));
    assert(0);
    break;
  }
}


/**
 * Callback from remote_dispatcher when a message is received.
 */
void message_service::remote_message_recv(const ID& tag_id,
                                          darc::buffer::shared_buffer data)
{
  boost::mutex::scoped_lock lock(mutex_);

  dispatcher_list_type::iterator elem =
    dispatcher_list_.find(tag_id);
  if(elem != dispatcher_list_.end())
  {
    elem->second->remote_message_recv(tag_id, data);
  }
  else
  {
    // silently ignore
//    iris::glog<iris::Warning>("message_service: remote msg for unknown tag id",
//                              "tag_id", iris::arg<ID>(tag_id));
  }
}

void message_service::peer_connected_handler(const ID& peer_id)
{
  for(dispatcher_list_type::iterator it = dispatcher_list_.begin();
      it != dispatcher_list_.end();
      it++)
  {
    it->second->inform_remote_peer(peer_id);
  }
}

void message_service::peer_disconnected_handler(const ID& peer_id)
{

}

void message_service::send_subscription(const ID& tag_id, const ID& peer_id)
{
  payload_header_packet hdr;
  hdr.payload_type = subscribe_packet::payload_id;
  outbound_data<serializer::boost_serializer, payload_header_packet> o_hdr(hdr);

  subscribe_packet sub(tag_id);
  outbound_data<serializer::boost_serializer, subscribe_packet> o_sub(sub);

  outbound_pair o_combined(o_hdr, o_sub);

  send_to(peer_id, o_combined);
}

void message_service::send_publish(const ID& tag_id, const ID& peer_id)
{
  payload_header_packet hdr;
  hdr.payload_type = publish_packet::payload_id;
  outbound_data<serializer::boost_serializer, payload_header_packet> o_hdr(hdr);

  publish_packet pub(tag_id);
  outbound_data<serializer::boost_serializer, publish_packet> o_pub(pub);

  outbound_pair o_combined(o_hdr, o_pub);

  send_to(peer_id, o_combined);
}

}
}
