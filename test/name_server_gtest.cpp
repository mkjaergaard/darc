#include <gtest/gtest.h>

#include <darc/test/two_peer_sim.hpp>
#include <darc/ns_service.hpp>

class NameServerTest : public darc::test::two_peer_sim, public testing::Test
{
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
  Step("Register NS1 tag");
  darc::tag_handle t1 = ns1.register_tag(ns1.root(), "myns/Tag1");

  Step("NS1 Content");
  ns1.print_tree();

  Step("Set Tag Callback");
  t1->connect_listener(boost::bind(callback, _1, _2));

  Step("Connect NS1<->NS2");
  ns1.connect(peer2.id());

  Step("NS2 Content");
  ns2.print_tree();

  Step("Register NS1 tag");
  darc::tag_handle t2_1 = ns2.register_tag(ns2.root(), "Tag1");

  Step("Register Namespace");
  darc::namespace_handle n2_1 = ns2.register_namespace(ns2.root(), "NS1");

  Step("Register NS1 tag2");
  darc::tag_handle t2_2 = ns2.register_tag(n2_1, "Tag2");

  Step("NS1 Content");
  ns1.print_tree();
  Step("NS2 Content");
  ns2.print_tree();

}
