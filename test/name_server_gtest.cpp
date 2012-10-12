#include <gtest/gtest.h>

#include <darc/ns_service.hpp>

class NameServerTest : public testing::Test
{
public:
//  boost::asio::io_service io_service_;

  darc::distributed_container::container_manager mngr1;
  darc::distributed_container::container_manager mngr2;
  darc::ID node1_id;
  darc::ID node2_id;
  darc::ns_service ns1;
  darc::ns_service ns2;

  NameServerTest() :
    mngr1(boost::bind(&NameServerTest::send_to_node2_, this, _1, _2)),
    mngr2(boost::bind(&NameServerTest::send_to_node1_, this, _1, _2)),
    node1_id(darc::ID::create()),
    node2_id(darc::ID::create()),
    ns1(&mngr1),
    ns2(&mngr2)
  {
    ns1.set_send_to_function(
      boost::bind(&NameServerTest::send_to_node2, this, _1, _2, _3));
    ns2.set_send_to_function(
      boost::bind(&NameServerTest::send_to_node1, this, _1, _2, _3));
  }

  void send_to_node1(const darc::ID& destination,
		     uint32_t service,
		     darc::buffer::shared_buffer data)
  {
    beam::glog<beam::Info>("Data Received from node 2---");
    ns1.recv(node2_id, service, data);
  }

  void send_to_node2(const darc::ID& destination,
		     uint32_t service,
		     darc::buffer::shared_buffer data)
  {
    beam::glog<beam::Info>("Data Received from node 1---");
    ns2.recv(node1_id, service, data);
  }

  void send_to_node1_(const darc::ID& destination, darc::buffer::shared_buffer data)
  {
    beam::glog<beam::Info>("Data Received from node 2");
    mngr1.recv(node2_id, data);
  }

  void send_to_node2_(const darc::ID& destination, darc::buffer::shared_buffer data)
  {
    beam::glog<beam::Info>("Data Received from node 1");
    mngr2.recv(node1_id, data);
  }
};

void callback(const darc::ID& id1, const darc::ID& id2)
{
  beam::glog<beam::Info>("Callback",
			 "ID1", beam::arg<darc::ID>(id1),
			 "ID2", beam::arg<darc::ID>(id2));
}

TEST_F(NameServerTest, Create)
{
  darc::ID id1 = ns1.register_namespace(ns1.root_id_, "Test1");
  darc::ID id2 = ns1.register_namespace(ns1.root_id_, "Test2");
  darc::ID id1_1 = ns1.register_namespace(id1, "Test1_1");
  darc::ID id1_2 = ns1.register_namespace(id1, "Test1_2");
  darc::ID id2_1 = ns1.register_namespace(id2, "Test2_1");
  darc::tag_handle t1 = ns1.register_tag(id1, "Tag1");

  t1->connect(boost::bind(callback, _1, _2));

  ns1.print_tree();

  darc::ID id1_ = ns2.register_namespace(ns2.root_id_, "Test555");
  ns1.connect(node2_id);//, ns1.list_.id());

  ns2.print_tree();
  darc::tag_handle t2 = ns2.register_tag(id1_, "Tag1");
  ns2.print_tree();

}
