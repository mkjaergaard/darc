#include <gtest/gtest.h>

#include <darc/test/two_peer_sim.hpp>
#include <darc/test/callback_monitor.hpp>
#include <darc/ns_service.hpp>

class NameServerTest : public darc::test::two_peer_sim, public testing::Test
{
protected:
  typedef darc::test::callback_monitor<darc::ID, darc::ID, darc::ID> tag_callback_monitor_type;

public:
  darc::distributed_container::container_manager mngr1;
  darc::distributed_container::container_manager mngr2;
  darc::ns_service ns1;
  darc::ns_service ns2;

  NameServerTest() :
    mngr1(peer1),
    mngr2(peer2),
    ns1(peer1, &mngr1),
    ns2(peer2, &mngr2)
  {
    beam::glog<beam::Info>("Peers",
                           "Peer1", beam::arg<darc::ID>(peer1.id()),
                           "Peer2", beam::arg<darc::ID>(peer2.id()));
  }
};

void callback(const darc::ID& id1, const darc::ID& id2)
{
  beam::glog<beam::Info>("Callback",
                         "ID1", beam::arg<darc::ID>(id1),
                         "ID2", beam::arg<darc::ID>(id2));
}

void Step(const std::string& title)
{
  std::cout << title << std::endl;
}

TEST_F(NameServerTest, Create)
{
  tag_callback_monitor_type p1_new_tag;
  tag_callback_monitor_type p2_new_tag;
  tag_callback_monitor_type p1_rem_tag;
  tag_callback_monitor_type p2_rem_tag;

  Step("Peer1: Register Tag1");
  darc::tag_handle t1_1 = ns1.register_tag(ns1.root(), "Tag1");
  t1_1->connect_new_tag_listener(p1_new_tag.functor());
  t1_1->connect_removed_tag_listener(p1_rem_tag.functor());

  EXPECT_EQ(0, p1_new_tag.num_callbacks());
  EXPECT_EQ(0, p2_new_tag.num_callbacks());
  EXPECT_EQ(0, p1_rem_tag.num_callbacks());
  EXPECT_EQ(0, p2_rem_tag.num_callbacks());

  Step("Connect P1<->P2");
  peer1.peer_connected(peer2.id());
  peer2.peer_connected(peer1.id());

  EXPECT_EQ(0, p1_new_tag.num_callbacks());
  EXPECT_EQ(0, p2_new_tag.num_callbacks());
  EXPECT_EQ(0, p1_rem_tag.num_callbacks());
  EXPECT_EQ(0, p2_rem_tag.num_callbacks());

  Step("Peer2: Register Tag1");
  darc::tag_handle t2_1 = ns2.register_tag(ns2.root(), "Tag1");
  t2_1->connect_new_tag_listener(p2_new_tag.functor());
  t2_1->connect_removed_tag_listener(p2_rem_tag.functor());

  EXPECT_EQ(1, p1_new_tag.num_callbacks());
  EXPECT_EQ(0, p2_new_tag.num_callbacks()); // should be 1
  EXPECT_EQ(0, p1_rem_tag.num_callbacks());
  EXPECT_EQ(0, p2_rem_tag.num_callbacks());

  t1_1.reset();
  EXPECT_EQ(0, p1_new_tag.num_callbacks());
  EXPECT_EQ(0, p2_new_tag.num_callbacks());
  EXPECT_EQ(1, p1_rem_tag.num_callbacks());
  EXPECT_EQ(1, p2_rem_tag.num_callbacks());

  Step("Disconnect P1<->P2");
  peer1.peer_disconnected(peer2.id());
  peer2.peer_disconnected(peer1.id());

  EXPECT_EQ(0, p1_new_tag.num_callbacks());
  EXPECT_EQ(0, p2_new_tag.num_callbacks());
  EXPECT_EQ(1, p1_rem_tag.num_callbacks());
  EXPECT_EQ(1, p2_rem_tag.num_callbacks());

  sleep(1);

}
