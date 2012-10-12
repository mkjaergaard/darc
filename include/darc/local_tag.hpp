#pragma once

#include <string>
#include <boost/signals.hpp>
#include <boost/signals/connection.hpp>
#include <darc/id.hpp>

namespace darc
{
class ns_service;

// manages lifetime of a local tag
class local_tag
{
protected:
  ns_service * ns_service_;
  ID id_;
  ID parent_ns_id_;
  std::string name_;

public:
  typedef void (callback_type)(const ID&, const ID&);
  typedef boost::function<callback_type> functor_type;

protected:
  typedef boost::signal<callback_type> signal_type;
  signal_type listeners_signal_;

public:
  local_tag(ns_service *,
	    const std::string name,
	    const ID& parent_ns_id);
  ~local_tag();

  void trigger_new_tag(const ID& tag_id, const ID& remote_instance)
  {
    listeners_signal_(id_, tag_id);
  }

  boost::signals::connection connect_listener(boost::function<callback_type> listener)
  {
    return listeners_signal_.connect(listener);
  }

  const ID& id() const
  {
    return id_;
  }

  const ID& namespace_id() const
  {
    return parent_ns_id_;
  }

  const std::string& name() const
  {
    return name_;
  }

};

typedef boost::shared_ptr<local_tag> local_tag_ptr;

}
