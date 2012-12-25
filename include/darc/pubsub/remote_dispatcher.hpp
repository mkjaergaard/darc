#pragma once

#include <boost/shared_ptr.hpp>
#include <darc/inbound_data.hpp>
#include <darc/serializer/boost.hpp>

#include <darc/payload_header_packet.hpp>
#include <darc/pubsub/message_packet.hpp>
#include <darc/pubsub/subscribe_packet.hpp>
#include <darc/peer.hpp>

namespace darc
{
namespace pubsub
{

class message_service; //fwd

class remote_dispatcher
{
protected:
  // Datastructure stuff, refactor out
  typedef std::pair</*peer*/ID, /*tag*/ID> remote_tag_type;
  typedef std::set<remote_tag_type> remote_tag_list_type;
  typedef boost::shared_ptr<remote_tag_list_type> remote_tag_list_type_ptr;
  typedef std::map</*tag*/ID, remote_tag_list_type_ptr> remote_list_type;

  remote_list_type list_;
  message_service * parent_service_;

protected:
  void handle_subscribe_packet(const darc::ID& src_peer_id,
                               darc::buffer::shared_buffer data)
  {
  }

  void handle_message_packet(const darc::ID& src_peer_id,
                             darc::buffer::shared_buffer data);

public:
  remote_dispatcher(message_service* parent) :
    parent_service_(parent)
  {
  }

  void recv(const darc::ID& src_peer_id,
            darc::buffer::shared_buffer data)
  {
    darc::inbound_data<darc::serializer::boost_serializer,
                       payload_header_packet> payload_type_i(data);
    switch(payload_type_i.get().payload_type)
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
      beam::glog<beam::Fatal>
        ("Unknown payload",
         "payload_id:", beam::arg<int>(payload_type_i.get().payload_type));
      assert(0);
      break;
    }
  }

  void new_tag_event(ID tag_id,
                     ID alias_id,
                     ID peer_id)
  {
    remote_list_type::iterator item = list_.find(tag_id);
    if(item != list_.end())
    {
      remote_tag_list_type& l = *(item->second);
      l.insert(remote_tag_type(peer_id, alias_id));
      beam::glog<beam::Info>("remote: new_tag", "tag_id", beam::arg<ID>(tag_id),
                                                "peer_id", beam::arg<ID>(peer_id));
    }
    else
    {
      remote_tag_list_type_ptr lp = boost::make_shared<remote_tag_list_type>();
      lp->insert(remote_tag_type(peer_id, alias_id));
      list_.insert(remote_list_type::value_type(tag_id, lp));
    }
  }

  void removed_tag_event(ID tag_id,
                         ID alias_id,
                         ID peer_id)
  {
    remote_list_type::iterator item = list_.find(tag_id);
    if(item != list_.end())
    {
      remote_tag_type entry_to_remove(peer_id, alias_id);
      remote_tag_list_type& l = *(item->second);
      l.erase(entry_to_remove);
    }
  }

  template<typename T>
  void dispatch_remotely(const ID& tag_id, const boost::shared_ptr<const T> &msg)
  {
    //send_msg(tag_id, ID::null(), msg);
    remote_list_type::iterator item = list_.find(tag_id);
    if(item != list_.end())
    {
      for(remote_tag_list_type::iterator it = item->second->begin();
          it != item->second->end();
          it++)
      {
        // todo: here we send a copy to all
        send_msg(/*tag_id*/it->second, it->first, msg);
      }
    }
  }

  void send_to(const ID& peer_id, const outbound_data_base& data);

  template<typename T>
  void send_msg(const ID& tag_id, const ID& peer_id, const boost::shared_ptr<const T> &msg)
  {
    payload_header_packet hdr;
    hdr.payload_type = message_packet::payload_id;
    outbound_data<serializer::boost_serializer, payload_header_packet> o_hdr(hdr);

    message_packet msg_hdr(tag_id);
    outbound_data<serializer::boost_serializer, message_packet> o_msg_hdr(msg_hdr);

    outbound_ptr<serializer::boost_serializer, T> o_msg(msg);

    outbound_pair o_pair1(o_hdr, o_msg_hdr);
    outbound_pair o_pair2(o_pair1, o_msg);

    send_to(peer_id, o_pair2);
  }

};

}
}
