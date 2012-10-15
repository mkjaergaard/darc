#pragma once

#include <darc/id.hpp>
#include <darc/peer_service.hpp>
#include <darc/distributed_container/shared_set.hpp>
#include <darc/local_tag.hpp>
#include <darc/tag_handle.hpp>
#include <darc/entry.hpp>
#include <darc/ns_connect_packet.hpp>
#include <darc/ns_connect_reply_packet.hpp>
#include <darc/payload_header_packet.hpp>
#include <darc/buffer/const_size_buffer.hpp>
#include <darc/local_ns.hpp>

namespace darc
{

class ns_service : public darc::peer_service
{
protected:
  const static int service_type_id = 55;

  typedef std::map<ID, local_tag_ptr> local_tag_list_type;
  local_tag_list_type local_tag_list_;

  local_ns_ptr root_ns_;

public:
  distributed_container::container_manager * container_manager_;

  ///////////////////////
  // Debug thingy
public:

  // søger på den hårde måde....
  local_tag_ptr has_local_tag(const ID& list_id, const std::string& name)
  {
    for(local_tag_list_type::iterator it = local_tag_list_.begin();
	it != local_tag_list_.end();
	it++)
    {
      const local_tag_ptr& lt = it->second;
      if(lt->name() == name)
      {
	return it->second;
      }
    }
    return local_tag_ptr();
  }


  void print_tree()
  {
/*
    std::cout << " . " << root_id_.short_string() << std::endl;
    for(list_type::iterator it = list_.list().begin();
	it != list_.list().end();
	it++)
    {
      entry& e = it->second.second;
      if(e.type == entry::namespace_type)
      {
	std::cout << " N " <<
	  it->first.short_string() << " " <<
	  it->second.first.short_string() << " " <<
	  e.name << std::endl;
      }
      else if(e.type == entry::tag_type)
      {
	std::cout << " T " <<
	  it->first.short_string() << " " <<
	  it->second.first.short_string() << " " <<
	  e.name << std::endl;
      }
    }
*/
  }

  void callback(const darc::ID& instance, const darc::ID& owner, const ID& key, const entry& value)
  {
/*
    beam::glog<beam::Info>("New Item Callback",
			   "instance", beam::arg<darc::ID>(instance),
			   "owner", beam::arg<darc::ID>(owner),
			   "key", beam::arg<ID>(key),
			   "value", beam::arg<entry>(value));
*/
    switch(value.type)
    {
    case entry::tag_type:
    {
      beam::glog<beam::Info>("New Tag");
      local_tag_ptr tag = has_local_tag(instance, value.name);
      if(tag.get() != 0)
      {
	beam::glog<beam::Info>("Trigger New Tag");
	tag->trigger_new_tag(key, instance);
      }
    }
    break;

    }

  }

public:
  ns_service(distributed_container::container_manager * container_manager) :
    container_manager_(container_manager),
    root_ns_(boost::make_shared<local_ns>(this, (local_ns*)0, std::string(".")))
  {
//    list_.attach(container_manager);
//    list_.signal_.connect(boost::bind(&ns_service::callback, this, _1, _2, _3, _4));
  }

  void connect_list(const ID& location_id, const ID& instance_id)
  {
//    list_.connect(location_id, instance_id);
  }

  // used by local_tag
  void add_entry(const ID& id, const entry& e)
  {
//    list_.insert(id, e); // select from ns id
  }

  void remove_entry(const ID&)
  {
  }

  tag_handle register_tag(const ID& ns_id, const std::string& name)
  {
    // todo: Check for existing
    local_tag_ptr my_tag = boost::make_shared<local_tag>(this, name, ns_id);
    local_tag_list_.insert(local_tag_list_type::value_type(my_tag->id(),
							   my_tag));

    return boost::make_shared<tag_handle_impl>(boost::ref(my_tag));
  }
/*
  void register_tag_link(const ID& parent_ns_id1, const std::string& name1,
			 const ID& parent_ns_id2, const std::string& name2)
  {
    ID id1 = ID::create();
    ID id2 = ID::create();

    entry e1(entry::tag_link_type);
    e1.name = name1;
    e1.parent_id = parent_ns_id1;
    e1.target_id = id2;

    entry e2(entry::tag_link_type);
    e2.name = name2;
    e2.parent_id = parent_ns_id2;
    e2.target_id = id1;

    list_.insert(id1, e1);
    list_.insert(id2, e2);
  }
*/
  const ID register_namespace(const ID& ns_id, const std::string& name)
  {
/*
    // todo: Check for existing
    entry e(entry::namespace_type);
    e.name = name;
    ID id = ID::create();
    list_.insert(id, e);
    return id;
*/
  }

  void handle_connect(const ID& src_peer_id, darc::buffer::shared_buffer data)
  {
    inbound_data<serializer::boost_serializer, ns_connect_packet> i_connect(data);
    connect_list(src_peer_id, i_connect.get().root_ns_list_id);
  }

  void handle_connect_reply(const ID& src_peer_id, darc::buffer::shared_buffer data)
  {
    inbound_data<serializer::boost_serializer, ns_connect_reply_packet> i_connect(data);

//    connect_list(src_peer_id, i_connect.get().root_ns_list_id);
  }

  void recv(const darc::ID& src_peer_id,
	    service_type,
	    darc::buffer::shared_buffer data)
  {
    inbound_data<serializer::boost_serializer, payload_header_packet> i_hdr(data);

    switch(i_hdr.get().payload_type)
    {
    case ns_connect_packet::payload_id:
    {
      handle_connect(src_peer_id, data);
      break;
    }
    case ns_connect_reply_packet::payload_id:
      handle_connect_reply(src_peer_id, data);
      break;
    default:
      assert(false);
    }
  }

  void connect(const ID& dest_peer_id)
  {
/*
    buffer::shared_buffer buffer = boost::make_shared<buffer::const_size_buffer>(2048); // todo

    payload_header_packet hdr;
    hdr.payload_type = ns_connect_packet::payload_id;
    outbound_data<serializer::boost_serializer, payload_header_packet> o_hdr(hdr);

    ns_connect_packet cnt;
    cnt.root_ns_id = root_id_;
    cnt.root_ns_list_id = list_.id();
    outbound_data<serializer::boost_serializer, ns_connect_packet> o_cnt(cnt);

    outbound_pair o_pair(o_hdr, o_cnt);

    o_pair.pack(buffer);

    send_to_function_(dest_peer_id, service_type_id, buffer);
    // send
    */
  }

};

}
