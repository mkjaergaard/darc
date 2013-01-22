#pragma once

#include <boost/signals/connection.hpp>
#include <darc/id.hpp>
#include <darc/peer_service.hpp>
#include <darc/local_tag.hpp>
#include <darc/tag_handle.hpp>

namespace darc
{

class ns_service : public darc::peer_service
{
protected:
  boost::signals::connection peer_connected_connection_;
  boost::signals::connection peer_disconnected_connection_;

  typedef std::map<local_tag*, local_tag_ptr> local_tag_list_type;
  local_tag_list_type local_tag_list_;

protected:
  void peer_connected_handler(const ID& peer_id)
  {
    for(local_tag_list_type::iterator it = local_tag_list_.begin();
	it != local_tag_list_.end();
	it++)
    {
      it->second->trigger_new_tag(it->second->id(), peer_id);
    }
  }

  void peer_disconnected_handler(const ID& peer_id)
  {
    for(local_tag_list_type::iterator it = local_tag_list_.begin();
	it != local_tag_list_.end();
	it++)
    {
      it->second->trigger_removed_tag(it->second->id(), peer_id);
    }
  }

public:
  ns_service(peer& p) :
    peer_service(p, 37)
  {
    peer_connected_connection_ =
      p.peer_connected_signal().connect(
	boost::bind(&ns_service::peer_connected_handler, this, _1));
    peer_disconnected_connection_ =
      p.peer_disconnected_signal().connect(
	boost::bind(&ns_service::peer_disconnected_handler, this, _1));
  }

  tag_handle register_tag(const std::string& full_name)
  {
    local_tag_ptr tag = local_tag::create(this, full_name);
    return boost::make_shared<tag_handle_impl>(boost::ref(tag));
  }

  void recv(const darc::ID& src_peer_id,
            darc::buffer::shared_buffer data)
  {
  }

  void attach_tag(local_tag_ptr tag)
  {
    local_tag_list_.insert(local_tag_list_type::value_type(tag.get(), tag));
  }

  void detach_tag(local_tag* tag)
  {
    local_tag_list_.erase(tag);
  }

};

}
