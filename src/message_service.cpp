#include <darc/pubsub/message_service.hpp>

namespace darc
{
namespace pubsub
{

message_service::message_service(peer& p, boost::asio::io_service& io_service, ns_service& ns_service) :
  peer_service(p, 13),
  io_service_(io_service),
  remote_dispatcher_(this),
  nameserver_(ns_service)
{
}

// /////////////////////////////////////////////
// Remap peer_service calls to remote_dispatcher
//
void message_service::recv(const darc::ID& src_peer_id,
                          darc::buffer::shared_buffer data)
{
  remote_dispatcher_.recv(src_peer_id, data);
}

void message_service::new_tag_event(ID tag_id,
                                   ID alias_id,
                                   ID peer_id)
{
  beam::glog<beam::Info>("tagEvent",
                         "tag_id", beam::arg<darc::ID>(tag_id),
                         "alias_id", beam::arg<darc::ID>(alias_id),
                         "peer_id", beam::arg<darc::ID>(peer_id));

  // Local alias
  if(peer_id == peer_service::peer_.id())
  {
    dispatcher_group_list_type::iterator elem1 = dispatcher_group_list_.find(tag_id);
    dispatcher_group_list_type::iterator elem2 = dispatcher_group_list_.find(alias_id);

    if(elem1 == dispatcher_group_list_.end() ||
       elem2 == dispatcher_group_list_.end())
    {
      beam::glog<beam::Warning>("tagEvent, but no dispatcher_group");
      return;
    }

    if(elem1->second.get() != elem2->second.get())
    {
      elem1->second->join(elem2->second);
      elem2->second = elem1->second;
    }
  }
  else
  {
    remote_dispatcher_.new_tag_event(tag_id, alias_id, peer_id);
  }
}

void message_service::remote_message_recv(const ID& tag_id,
                                         darc::buffer::shared_buffer data)
{
  dispatcher_group_list_type::iterator elem =
    dispatcher_group_list_.find(tag_id);
  if(elem != dispatcher_group_list_.end())
  {
    elem->second->remote_message_recv(tag_id, data);
  }
  else
  {
    beam::glog<beam::Warning>("message_service: remote msg for unknown tag id",
                              "tag_id", beam::arg<ID>(tag_id));
  }
}

// /////////////////////////////////////////////

void message_service::post_new_tag_event(ID tag_id,
                                        ID alias_id,
                                        ID peer_id)
{
  io_service_.post(boost::bind(&message_service::new_tag_event, this, tag_id, alias_id, peer_id));
}

}
}
