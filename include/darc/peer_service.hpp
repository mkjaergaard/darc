#pragma once

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <darc/buffer/shared_buffer.hpp>
#include <darc/id.hpp>
#include <darc/peer.hpp>

namespace darc
{

typedef uint32_t service_type;

class peer_service
{
protected:
  typedef boost::function<void(const darc::ID&,
                               service_type,
                               darc::buffer::shared_buffer)> send_to_function_type;

  peer& peer_;
  service_type service_id_;

public:
  virtual void recv(const darc::ID& src_peer_id,
                    darc::buffer::shared_buffer data) = 0;

  peer_service(darc::peer& p, service_type service_id) :
    peer_(p),
    service_id_(service_id)
  {
    peer_.attach(service_id, this);
  }

  virtual ~peer_service()
  {
  }

  void send_to(const ID& peer_id, const outbound_data_base& data)
  {
    peer_.send_to(peer_id, service_id_, data);
  }

};

}
