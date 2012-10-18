#pragma once

#include <boost/shared_ptr.hpp>
#include <darc/inbound_data.hpp>
#include <darc/serializer/boost.hpp>

#include <darc/payload_header_packet.hpp>
#include <darc/message_packet.hpp>
#include <darc/subscribe_packet.hpp>
#include <darc/peer.hpp>

namespace darc
{

class RemoteDispatcher
{
protected:
  // Datastructure stuff, refactor out
  typedef std::pair</*peer*/ID, /*tag*/ID> remote_tag_type;
  typedef std::set<remote_tag_type> remote_tag_list_type;
  typedef boost::shared_ptr<remote_tag_list_type> remote_tag_list_type_ptr;
  typedef std::map</*tag*/ID, remote_tag_list_type_ptr> remote_list_type;

  remote_list_type list_;
  peer& peer_;

protected:
  void handle_subscribe_packet(const darc::ID& src_peer_id,
			       darc::buffer::shared_buffer data)
  {
  }

  void handle_message_packet(const darc::ID& src_peer_id,
			     darc::buffer::shared_buffer data)
  {
  }

public:
  RemoteDispatcher(peer& p) :
    peer_(p)
  {
  }

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

  void new_tag_event(ID tag_id,
		     ID alias_id,
		     ID peer_id)
  {
    remote_list_type::iterator item = list_.find(tag_id);
    if(item != list_.end())
    {
      remote_tag_list_type& l = *(item->second);
      l.insert(remote_tag_type(peer_id, alias_id));
    }
    else
    {
      remote_tag_list_type_ptr lp = boost::make_shared<remote_tag_list_type>();
      lp->insert(remote_tag_type(peer_id, alias_id));
      list_.insert(remote_list_type::value_type(tag_id, lp));
    }
  }

  template<typename T>
  void dispatch_remotely(const ID& tag_id, const boost::shared_ptr<const T> &msg)
  {
    remote_list_type::iterator item = list_.find(tag_id);
    if(item != list_.end())
    {
      for(remote_tag_list_type::iterator it = item->second->begin();
	  it != item->second->end();
	  it++)
      {
	// todo: here we send a copy to all
	send_msg(tag_id, it->first, msg);
      }
    }
  }

  template<typename T>
  void send_msg(const ID& tag_id, const ID& peer_id, const boost::shared_ptr<const T> &msg)
  {

    // todo move to peer_service
    buffer::shared_buffer buffer = boost::make_shared<buffer::const_size_buffer>(1024*10); // todo

    payload_header_packet hdr;
    hdr.payload_type = message_packet::payload_id;
    outbound_data<serializer::boost_serializer, payload_header_packet> o_hdr(hdr);

    message_packet msg_hdr(tag_id);
    outbound_data<serializer::boost_serializer, message_packet> o_msg_hdr(msg_hdr);

    outbound_ptr<serializer::boost_serializer, T> o_msg(msg);

    outbound_pair o_pair1(o_hdr, o_msg_hdr);
    outbound_pair o_pair2(o_pair1, o_msg);

    o_pair2.pack(buffer);

    peer_.send_to(peer_id, 52, buffer);
  }

};

}
