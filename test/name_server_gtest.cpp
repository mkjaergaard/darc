#include <gtest/gtest.h>

#include <darc/name_server.hpp>

class NameServerTest : public testing::Test
{
public:
//  boost::asio::io_service io_service_;

  darc::distributed_container::container_manager mngr1;
  darc::distributed_container::container_manager mngr2;
  darc::ID node1_id;
  darc::ID node2_id;

  NameServerTest() :
    mngr1(boost::bind(&NameServerTest::send_to_node2, this, _1, _2)),
    mngr2(boost::bind(&NameServerTest::send_to_node1, this, _1, _2)),
    node1_id(darc::ID::create()),
    node2_id(darc::ID::create())
  {
  }

  void send_to_node1(const darc::ID& destination, darc::buffer::shared_buffer data)
  {
    beam::glog<beam::Info>("Data Received from node 2");
    mngr1.recv(node2_id, data);
  }

  void send_to_node2(const darc::ID& destination, darc::buffer::shared_buffer data)
  {
    beam::glog<beam::Info>("Data Received from node 1");
    mngr2.recv(node1_id, data);
  }
};

TEST_F(NameServerTest, Create)
{
  darc::ns_service ns1(&mngr1);
  darc::ID id1 = ns1.register_namespace(ns1.root_id_, "Test1");
  darc::ID id2 = ns1.register_namespace(ns1.root_id_, "Test2");
  darc::ID id1_1 = ns1.register_namespace(id1, "Test1_1");
  darc::ID id1_2 = ns1.register_namespace(id1, "Test1_2");
  darc::ID id2_1 = ns1.register_namespace(id2, "Test2_1");
  ns1.print_tree();

}

