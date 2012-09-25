#pragma once

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <darc/buffer/shared_buffer.hpp>
#include <darc/id.hpp>

namespace darc
{

class peer;

typedef uint32_t service_type;

class peer_service
{
protected:
  typedef boost::function<void(const darc::ID&,
			       service_type,
			       darc::buffer::shared_buffer)> send_to_function_type;
  typedef boost::function<void(const darc::ID&,
			       service_type,
			       darc::buffer::shared_buffer)> recv_function_type;

  send_to_function_type send_to_function_;

  virtual void recv(const darc::ID& src_peer_id,
		    service_type,
		    darc::buffer::shared_buffer data) = 0;

  peer_service() {}
  peer_service(darc::peer& p) {}

  void set_send_to_function(send_to_function_type send_to_function)
  {
    send_to_function_ = send_to_function;
  }

  recv_function_type get_recv_function()
  {
    return boost::bind(&peer_service::recv, this, _1, _2, _3);
  }

};

}
