#pragma once

#include <string>
#include <boost/signals.hpp>
#include <boost/signals/connection.hpp>
#include <darc/id.hpp>
#include <darc/distributed_container/shared_set.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <beam/glog.hpp>
#include <darc/entry.hpp>
#include <darc/local_tag.hpp>

namespace darc
{

class ns_service;
class local_ns;
typedef boost::shared_ptr<local_ns> local_ns_ptr;

// manages lifetime of a local namespace
class local_ns : public boost::enable_shared_from_this<local_ns>
{
protected:
public: //for printing
  typedef distributed_container::shared_set<ID, entry> list_type;
  list_type list_; // the shared list!

  ns_service * ns_service_;
  local_ns * parent_;

  ID id_;
  std::string name_;

public: //for printing
  // Child Tags
  typedef std::map<ID, local_tag_ptr> local_tag_list_type;
  local_tag_list_type local_tag_list_;

  // Child NS
  typedef std::map<ID, local_ns_ptr> local_ns_list_type;
  local_ns_list_type local_ns_list_;

public:
  local_ns(ns_service * service,
           distributed_container::container_manager * container_manager,
           const std::string name,
           local_ns* parent);

  static local_ns_ptr create(ns_service *,
                             distributed_container::container_manager * container_manager,
                             const std::string name = "",
                             local_ns * parent = 0);

  ~local_ns()
  {
  }

  void callback(const darc::ID& instance, const darc::ID& owner, const ID& key, const entry& value)
  {
    if(owner == list_.id())
    {
      return;
    }
    beam::glog<beam::Trace>("New Item Callback",
                            "instance", beam::arg<darc::ID>(instance),
                            "owner", beam::arg<darc::ID>(owner),
                            "key", beam::arg<ID>(key),
                            "value", beam::arg<entry>(value));

    switch(value.type)
    {
    case entry::tag_type:
    {
      // Check if we have a local tag with he name, and trigger the callback
      beam::glog<beam::Info>("New Tag Callback");
      local_tag_ptr tag = find_local_tag(value.name);
      if(tag.get() != 0)
      {
        beam::glog<beam::Info>("Trigger New Tag");
        tag->trigger_new_tag(key, list_.get_peer_id(owner));
      }
    }
    break;
    case entry::namespace_type:
    {
      // Check if we have a local ns, and add the remote instance
      beam::glog<beam::Info>("New NS Callback");
      local_ns_ptr ns = find_local_ns(value.name);
      if(ns.get() != 0)
      {
        ns->connect_list_instance(owner, value.list_instance_id);
      }
    }
    break;
    }
  }

  // Remote stuff
  //
  void connect_list_instance(const ID& peer_id, const ID& instance_id)
  {
    list_.connect(peer_id, instance_id);
  }

  // Find method
  // Find local tag based on name
  local_tag_ptr find_local_tag(const std::string& name)
  {
    for(local_tag_list_type::iterator it = local_tag_list_.begin();
        it != local_tag_list_.end();
        it++)
    {
      if(it->second->name() == name)
      {
        return it->second;
      }
    }
    return local_tag_ptr(); // Return empty ptr in case of not found
  }

  // Find local tag based on name
  local_ns_ptr find_local_ns(const std::string& name)
  {
    for(local_ns_list_type::iterator it = local_ns_list_.begin();
        it != local_ns_list_.end();
        it++)
    {
      if(it->second->name() == name)
      {
        return it->second;
      }
    }
    return local_ns_ptr(); // Return empty ptr in case of not found
  }


  // Tree Methods
  // called by the child NS
  void attach_child_ns(local_ns_ptr child)
  {
    local_ns_list_.insert(
      local_ns_list_type::value_type(child->id(),
                                     child));

    entry e(entry::namespace_type);
    e.name = child->name();
    e.list_instance_id = child->list_instance_id();

    list_.insert(child->id(), e);
  }

  // called by the child tag
  void attach_child_tag(local_tag_ptr child)
  {
    local_tag_list_.insert(
      local_tag_list_type::value_type(child->id(),
                                      child));

    entry e(entry::tag_type);
    e.name = child->name();

    list_.insert(child->id(), e);
  }

  // Getters
  const ID& id() const
  {
    return id_;
  }
/*
  const ID& namespace_id() const
  {
  return parent_ns_id_;
  }
*/
  const std::string& name() const
  {
    return name_;
  }

  const ID& list_instance_id() const
  {
    return list_.id();
  }

};

typedef boost::shared_ptr<local_tag> local_tag_ptr;

}
