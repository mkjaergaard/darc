#include <gtest/gtest.h>

#include <darc/distributed_container/shared_set.hpp>

#include <boost/bind.hpp>
#include <beam/glog.hpp>
#include <darc/id_arg.hpp>

class SharedSetTest : public testing::Test
{
public:
  darc::distributed_container::container_manager mngr1;
  darc::distributed_container::container_manager mngr2;
  darc::ID node1_id;
  darc::ID node2_id;

  SharedSetTest() :
    mngr1(boost::bind(&SharedSetTest::send_to_node2, this, _1, _2)),
    mngr2(boost::bind(&SharedSetTest::send_to_node1, this, _1, _2)),
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

bool equal_(const darc::distributed_container::shared_set<std::string, uint32_t>& set1,
	   const darc::distributed_container::shared_set<std::string, uint32_t>& set2)
{
  typedef darc::distributed_container::shared_set<std::string, uint32_t> set_type;
  typedef set_type::list_type::const_iterator i_type;
  for(i_type it = set1.list().begin();
      it != set1.list().end();
      it++)
  {
    i_type item = set2.list().find(it->first);
    if(item == set2.list().end())
    {
      return false;
    }
  }
  return true;
}
bool equal(const darc::distributed_container::shared_set<std::string, uint32_t>& set1,
	   const darc::distributed_container::shared_set<std::string, uint32_t>& set2)
{
  return equal_(set1, set2) && equal_(set2, set1);
}


TEST_F(SharedSetTest, Subscribe)
{
  darc::distributed_container::shared_set<std::string, uint32_t> my_set1;
  darc::distributed_container::shared_set<std::string, uint32_t> my_set2;

  beam::glog<beam::Info>("Shared Set Created",
			 "Node1", beam::arg<darc::ID>(node1_id),
			 "Node2", beam::arg<darc::ID>(node2_id),
			 "Set1", beam::arg<darc::ID>(my_set1.id()),
			 "Set2", beam::arg<darc::ID>(my_set2.id()));

  my_set1.attach(&mngr1);
  my_set2.attach(&mngr2);

  my_set1.insert("key3", 3);
  my_set1.insert("key4", 4);

  my_set1.connect(node2_id, my_set2.id());

  EXPECT_EQ(equal(my_set1, my_set2), true);

  my_set1.insert("key1", 1);

  EXPECT_EQ(equal(my_set1, my_set2), true);

  my_set2.insert("key2", 2);

  EXPECT_EQ(equal(my_set1, my_set2), true);

}

