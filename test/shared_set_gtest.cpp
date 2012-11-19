#include <gtest/gtest.h>

#include <darc/test/two_peer_sim.hpp>
#include <darc/test/step.hpp>
#include <darc/distributed_container/shared_set.hpp>

#include <boost/bind.hpp>
#include <beam/glog.hpp>
#include <darc/id_arg.hpp>

class SharedSetTest : public darc::test::two_peer_sim, public testing::Test
{
public:
  darc::distributed_container::container_manager mngr1;
  darc::distributed_container::container_manager mngr2;

  SharedSetTest() :
    mngr1(peer1),
    mngr2(peer2)
  {
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

template<typename Key, typename T>
void callback(const darc::ID& instance, const darc::ID& owner, const Key& key, const T& value)
{
  beam::glog<beam::Info>("New Item Callback",
                         "instance", beam::arg<darc::ID>(instance),
                         "owner", beam::arg<darc::ID>(owner),
                         "key", beam::arg<Key>(key),
                         "value", beam::arg<T>(value));
}

TEST_F(SharedSetTest, Subscribe)
{
  darc::test::step("Creating Shared Sets");
  darc::distributed_container::shared_set<std::string, uint32_t> my_set1;
  darc::distributed_container::shared_set<std::string, uint32_t> my_set2;

  beam::glog<beam::Info>("Shared Set Created",
                         "Node1", beam::arg<darc::ID>(peer1.id()),
                         "Node2", beam::arg<darc::ID>(peer2.id()),
                         "Set1", beam::arg<darc::ID>(my_set1.id()),
                         "Set2", beam::arg<darc::ID>(my_set2.id()));

  darc::test::step("Attach to managers");
  my_set1.attach(&mngr1);
  my_set2.attach(&mngr2);

  darc::test::step("Connect Listener");
  my_set1.signal_.connect(boost::bind(&callback<std::string, uint32_t>, _1, _2, _3, _4));

  darc::test::step("Connect Set1->Set2");
  my_set1.connect(peer2.id(), my_set2.id());

  darc::test::step("Insert key3 + key4 into Set1");
  my_set1.insert("key3", 3);
  my_set1.insert("key4", 4);

  EXPECT_EQ(equal(my_set1, my_set2), true);

  darc::test::step("Insert key1 into Set1");
  my_set1.insert("key1", 1);

  EXPECT_EQ(equal(my_set1, my_set2), true);

  darc::test::step("Insert key2 into Set2");
  my_set2.insert("key2", 2);

  EXPECT_EQ(equal(my_set1, my_set2), true);

}
