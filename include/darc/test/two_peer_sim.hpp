#include <boost/bind.hpp>
#include <iris/glog.hpp>
#include <darc/peer/peer.hpp>
#include <darc/id_arg.hpp>

#include <darc/buffer/raw_buffer.hpp>

namespace darc
{
namespace test
{

class two_peer_sim
{
public:
  darc::peer peer1;
  darc::peer peer2;

  two_peer_sim()
  {
    peer1.set_send_to_function(
      boost::bind(&two_peer_sim::send_to_node2, this, _1, _2));
    peer2.set_send_to_function(
      boost::bind(&two_peer_sim::send_to_node1, this, _1, _2));
  }

  void send_to_node1(const darc::ID& peer_id, darc::buffer::shared_buffer data)
  {
    iris::glog<iris::Info>("Data Received from node 2");
    peer1.recv(peer2.id(), boost::make_shared<darc::buffer::raw_buffer>(data->data(), 1024, 1024));
  }

  void send_to_node2(const darc::ID& peer_id, darc::buffer::shared_buffer data)
  {
    iris::glog<iris::Info>("Data Received from node 1");
    peer2.recv(peer1.id(), boost::make_shared<darc::buffer::raw_buffer>(data->data(), 1024, 1024));
  }
};

}
}
