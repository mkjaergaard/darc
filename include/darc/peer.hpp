#pragma once

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/signal.hpp>
#include <darc/buffer/shared_buffer.hpp>
#include <darc/buffer/const_size_buffer.hpp>
#include <darc/id.hpp>
#include <darc/service_header_packet.hpp>
#include <darc/inbound_data.hpp>
#include <darc/outbound_data.hpp>
#include <darc/serializer/boost.hpp>
#include <iris/glog.hpp>
#include <darc/id_arg.hpp>

namespace darc
{

typedef uint32_t service_type;

class peer_service;

class peer
{
protected:
  typedef std::map<service_type, peer_service*> service_list_type;

  typedef boost::function<void(const darc::ID&, darc::buffer::shared_buffer)> send_to_function_type;

  typedef boost::signal<void(const ID&)> peer_connected_signal_type;
  typedef boost::signal<void(const ID&)> peer_disconnected_signal_type;

  send_to_function_type send_to_function_;
  ID id_;
  service_list_type service_list_;

  peer_connected_signal_type peer_connected_signal_;
  peer_disconnected_signal_type peer_disconnected_signal_;

public:
  peer() :
    id_(ID::create())
  {}

  peer_connected_signal_type& peer_connected_signal()
  {
    return peer_connected_signal_;
  }

  peer_disconnected_signal_type& peer_disconnected_signal()
  {
    return peer_disconnected_signal_;
  }

  void send_to(const ID& peer_id, service_type service, const outbound_data_base& data)
  {
    service_header_packet header;
    header.service_type = service;
    outbound_data<darc::serializer::boost_serializer, service_header_packet> o_header(header);

    outbound_pair o_merge(o_header, data);

    buffer::shared_buffer buffer = boost::make_shared<buffer::const_size_buffer>(1024*10); // todo
    o_merge.pack(buffer);

    send_to_function_(peer_id, buffer);
  }

  void recv(const ID& src_peer_id, buffer::shared_buffer data);

  void peer_connected(const ID& peer_id)
  {
    iris::glog<iris::Info>("Peer Connected",
                           "peer_id", iris::arg<ID>(peer_id));

    peer_connected_signal_(peer_id);
  }

  void peer_disconnected(const ID& peer_id)
  {
    iris::glog<iris::Info>("Peer Disconnected",
                           "peer_id", iris::arg<ID>(peer_id));

    peer_disconnected_signal_(peer_id);
  }

  virtual void set_send_to_function(send_to_function_type send_to_function)
  {
    send_to_function_ = send_to_function;
  }

  const ID& id()
  {
    return id_;
  }

  void attach(service_type service_index, peer_service * service_instance)
  {
    assert(service_list_.find(service_index) == service_list_.end());
    service_list_.insert(
      service_list_type::value_type(service_index, service_instance));
  }

};

}
