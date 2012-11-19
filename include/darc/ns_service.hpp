#pragma once

#include <boost/signals/connection.hpp>
#include <darc/id.hpp>
#include <darc/peer_service.hpp>
#include <darc/distributed_container/shared_set.hpp>
#include <darc/local_tag.hpp>
#include <darc/tag_handle.hpp>
#include <darc/namespace_handle.hpp>
#include <darc/entry.hpp>
#include <darc/ns_connect_packet.hpp>
#include <darc/ns_connect_reply_packet.hpp>
#include <darc/payload_header_packet.hpp>
#include <darc/local_ns.hpp>
#include <darc/tag_parser.hpp>

namespace darc
{

class ns_service : public darc::peer_service
{
protected:
  local_ns_ptr root_ns_;
  namespace_handle root_handle_;

  distributed_container::container_manager * container_manager_;

  boost::signals::connection peer_connected_connection_;

  ///////////////////////
  // Debug thingy
public:
  namespace_handle& root()
  {
    return root_handle_;
  }

  void print_tree()
  {
    print_ns(root_ns_, 1);
  }

  void print_ns(local_ns_ptr ns, int depth)
  {
    std::cout << depth - 1 << " N: " << ns->id().short_string() << " " << ns->name() << std::endl;

    for(local_ns::list_type::iterator it = ns->list_.list().begin();
        it != ns->list_.list().end();
        it++)
    {
      entry& e = it->second.second;
      if(e.type == entry::namespace_type)
      {
        std::cout << depth << " n: " <<
          it->first.short_string() << " " <<
          it->second.first.short_string() << " " <<
          e.name << std::endl;
      }
      else if(e.type == entry::tag_type)
      {
        std::cout << depth << " t: " <<
          it->first.short_string() << " " <<
          it->second.first.short_string() << " " <<
          e.name << std::endl;
      }
    }

    for(local_ns::local_ns_list_type::iterator it = ns->local_ns_list_.begin();
        it != ns->local_ns_list_.end();
        it++)
    {
      print_ns(it->second, depth+1);
    }
    for(local_ns::local_tag_list_type::iterator it = ns->local_tag_list_.begin();
        it != ns->local_tag_list_.end();
        it++)
    {
      std::cout << depth << " T: " << it->second->id().short_string() << " " << it->second->name() << std::endl;
    }
  }

protected:
  void peer_connected_handler(const ID& peer_id)
  {
    // lowest peer_id connects to the highest peer_id
    if(peer_.id() < peer_id)
    {
      connect(peer_id);
    }
  }

public:
  ns_service(peer& p, distributed_container::container_manager * container_manager) :
    peer_service(p, 37),
    container_manager_(container_manager),
    root_ns_(local_ns::create(this, container_manager, ".")),
    root_handle_(boost::make_shared<namespace_handle_impl>(boost::ref(root_ns_)))
  {
    peer_connected_connection_ = p.peer_connected_signal().connect(boost::bind(&ns_service::peer_connected_handler, this, _1));
  }

  tag_handle register_tag(namespace_handle ns, const std::string& full_name)
  {
    tag_parser p(full_name);

    for(tag_parser::namespace_list_type::const_iterator it = p.get_namespaces().begin();
        it != p.get_namespaces().end();
        it++)
    {
      ns = register_namespace_(ns, *it);
    }

    return register_tag_(ns, p.get_tag());
  }

  tag_handle register_tag_(namespace_handle ns, const std::string& name)
  {
    local_tag_ptr my_tag = ns->instance()->find_local_tag(name);
    if(my_tag.get() == 0)
    {
      my_tag = local_tag::create(this, name, ns->instance().get());
    }
    return boost::make_shared<tag_handle_impl>(boost::ref(my_tag));
  }

/*
  void register_tag_link(const ID& parent_ns_id1, const std::string& name1,
  const ID& parent_ns_id2, const std::string& name2)
  {
  ID id1 = ID::create();
  ID id2 = ID::create();

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

  namespace_handle register_namespace(namespace_handle ns, const std::string& full_name)
  {
    tag_parser p(full_name);

    for(tag_parser::namespace_list_type::const_iterator it = p.get_namespaces().begin();
        it != p.get_namespaces().end();
        it++)
    {
      ns = register_namespace_(ns, *it);
    }

    return register_namespace_(ns, p.get_tag());
  }

  namespace_handle register_namespace_(namespace_handle ns, const std::string& name)
  {
    local_ns_ptr my_ns = ns->instance()->find_local_ns(name);
    if(my_ns.get() == 0)
    {
      my_ns = local_ns::create(this, container_manager_, name, ns->instance().get());
    }
    return boost::make_shared<namespace_handle_impl>(boost::ref(my_ns));
  }

  void handle_connect(const ID& src_peer_id, darc::buffer::shared_buffer data)
  {
    inbound_data<serializer::boost_serializer, ns_connect_packet> i_connect(data);
    root_ns_->connect_list_instance(src_peer_id, i_connect.get().root_ns_list_id);
  }

  void handle_connect_reply(const ID& src_peer_id, darc::buffer::shared_buffer data)
  {
    inbound_data<serializer::boost_serializer, ns_connect_reply_packet> i_connect(data);

//    connect_list(src_peer_id, i_connect.get().root_ns_list_id);
  }

  void recv(const darc::ID& src_peer_id,
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
      break;
    }
  }

  void connect(const ID& dest_peer_id)
  {
    payload_header_packet hdr;
    hdr.payload_type = ns_connect_packet::payload_id;
    outbound_data<serializer::boost_serializer, payload_header_packet> o_hdr(hdr);

    ns_connect_packet cnt;
    cnt.root_ns_list_id = root_ns_->list_instance_id();
    outbound_data<serializer::boost_serializer, ns_connect_packet> o_cnt(cnt);

    outbound_pair o_pair(o_hdr, o_cnt);

    send_to(dest_peer_id, o_pair);
  }

};

}
