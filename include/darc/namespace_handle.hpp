#pragma once

#include <darc/id.hpp>
#include <darc/local_ns.hpp>

namespace darc
{

class namespace_handle_impl
{
protected:
  local_ns_ptr local_ns_;

public:
  namespace_handle_impl(local_ns_ptr& local_ns) :
    local_ns_(local_ns)
  {
  }

  const ID& id() const
  {
    return local_ns_->id();
  }

  const std::string& name() const
  {
    return local_ns_->name();
  }

  local_ns_ptr& instance()
  {
    return local_ns_;
  }
};

typedef boost::shared_ptr<namespace_handle_impl> namespace_handle;

}
