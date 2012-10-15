#pragma once

#include <string>
#include <boost/signals.hpp>
#include <boost/signals/connection.hpp>
#include <darc/id.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace darc
{

class ns_service;
class local_ns;
typedef boost::shared_ptr<local_ns> local_ns_ptr;

// manages lifetime of a local namespace
class local_ns : public boost::enable_shared_from_this<local_ns>
{
protected:
  typedef distributed_container::shared_set<ID, entry> list_type;
  list_type list_; // the shared list!

  ns_service * ns_service_;
  local_ns * parent_;
  ID id_;
  std::string name_;

  typedef std::map<ID, local_tag_ptr> local_tag_list_type;
  local_tag_list_type local_tag_list_;

  typedef std::map<ID, local_ns_ptr> local_ns_list_type;
  local_ns_list_type local_ns_list_;

public:
  local_ns(ns_service * service,
	   local_ns* parent,
	   const std::string name) :
    ns_service_(service),
    parent_(parent)
  {
    if(parent_ != 0)
    {
      parent_->attach_child_ns(shared_from_this());
    }
  }

  ~local_ns()
  {
  }

  void attach_child_ns(local_ns_ptr child)
  {
    local_ns_list_.insert(
      local_ns_list_type::value_type(child->id(),
				     child));
    // insert into the shared list also!
    // configure the shared list id!
  }

  void attach_child_tag(local_tag_ptr child)
  {
    local_tag_list_.insert(
      local_tag_list_type::value_type(child->id(),
				      child));
    // insert into the shared list also!
    // configure the shared list id!
  }

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

};

typedef boost::shared_ptr<local_tag> local_tag_ptr;

}
