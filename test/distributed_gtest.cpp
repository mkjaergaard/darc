#include <gtest/gtest.h>

#include <darc/distributed_container/container_manager.hpp>
#include <darc/distributed_container/published_set.hpp>
#include <darc/distributed_container/subscribed_set.hpp>

#include <boost/bind.hpp>
#include <beam/glog.hpp>

class DistributedTest : public testing::Test
{
public:
  darc::distributed_container::container_manager my_server_mngr;
  darc::distributed_container::container_manager my_client_mngr;
  darc::ID server_node_id;
  darc::ID client_node_id;

  DistributedTest() :
    my_server_mngr(boost::bind(&DistributedTest::send_to_client_node, this, _1, _2)),
    my_client_mngr(boost::bind(&DistributedTest::send_to_server_node, this, _1, _2)),
    server_node_id(darc::ID::create()),
    client_node_id(darc::ID::create())
  {
  }

  void send_to_client_node(const darc::ID& destination, darc::buffer::shared_buffer data)
  {
    beam::glog<beam::Info>("Data Received from Server");
    my_client_mngr.recv(client_node_id, data);
  }

  void send_to_server_node(const darc::ID& destination, darc::buffer::shared_buffer data)
  {
    beam::glog<beam::Info>("Data Received from Client");
    my_server_mngr.recv(server_node_id, data);
  }
};

TEST_F(DistributedTest, Subscribe)
{
  darc::distributed_container::published_set<uint32_t> my_vector;
  darc::distributed_container::subscribed_set<uint32_t> my_vector_client;

  my_vector.attach(&my_server_mngr);
  my_vector_client.attach(&my_client_mngr);

  my_vector.insert(4);

  my_vector_client.subscribe(server_node_id, my_vector.id());

  my_vector.insert(5);
  my_vector.flush();

  my_vector.insert(6);
  my_vector.erase(4);
  my_vector.flush();

}
