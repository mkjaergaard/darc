#pragma once

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <darc/buffer/shared_buffer.hpp>
#include <darc/id.hpp>

namespace darc
{

typedef uint32_t service_type;

class peer
{
protected:
  typedef boost::function<void(const darc::ID&,
			       service_type,
			       darc::buffer::shared_buffer)> send_to_function_type;

  send_to_function_type send_to_function_;
  ID id_;

public:
  peer() :
    id_(ID::create())
  {}

  void send_to(const ID& peer_id, service_type service, darc::buffer::shared_buffer data)
  {
    send_to_function_(peer_id, service, data);
  }

  virtual void set_send_to_function(send_to_function_type send_to_function)
  {
    send_to_function_ = send_to_function;
  }

  const ID& id()
  {
    return id_;
  }

};

}
