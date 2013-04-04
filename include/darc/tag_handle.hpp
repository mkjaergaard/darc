#pragma once

#include <darc/id.hpp>
#include <darc/local_tag.hpp>

namespace darc
{

class tag_handle_impl
{
public:
  typedef local_tag::functor_type listener_type;

  boost::signals::connection new_tag_signal_connection_;
  boost::signals::connection removed_tag_signal_connection_;

protected:
  local_tag_ptr local_tag_;

public:
  tag_handle_impl(local_tag_ptr& local_tag) :
    local_tag_(local_tag)
  {
  }

  ~tag_handle_impl()
  {
    new_tag_signal_connection_.disconnect();
    removed_tag_signal_connection_.disconnect();
  }

  const ID& id() const
  {
    return local_tag_->id();
  }

  const std::string& name() const
  {
    return local_tag_->name();
  }

  void connect_new_tag_listener(local_tag::functor_type callback)
  {
    new_tag_signal_connection_ = local_tag_->new_tag_signal().connect(callback);
  }

  void connect_removed_tag_listener(local_tag::functor_type callback)
  {
    removed_tag_signal_connection_ = local_tag_->removed_tag_signal().connect(callback);
  }

  void reset()
  {
    new_tag_signal_connection_.disconnect();
    removed_tag_signal_connection_.disconnect();
    local_tag_.reset();
  }

};

typedef boost::shared_ptr<tag_handle_impl> tag_handle;

}
