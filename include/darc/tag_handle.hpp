#pragma once

#include <darc/id.hpp>
#include <darc/local_tag.hpp>

namespace darc
{

class tag_handle_impl
{
public:
  typedef local_tag::functor_type listener_type;

  boost::signals::connection connection_;

protected:
  local_tag_ptr local_tag_;

public:
  tag_handle_impl(local_tag_ptr& local_tag) :
    local_tag_(local_tag)
  {
  }

  const ID& id() const
  {
    return local_tag_->id();
  }

  const std::string& name() const
  {
    return local_tag_->name();
  }

  void connect_listener(local_tag::functor_type callback)
  {
    local_tag_->connect_listener(callback);
  }

};

typedef boost::shared_ptr<tag_handle_impl> tag_handle;

}
