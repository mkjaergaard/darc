#include <gtest/gtest.h>

#include <darc/test/two_peer_sim.hpp>
#include <darc/publisher.hpp>
#include <darc/subscriber.hpp>
#include <darc/local_dispatcher.hpp>
#include <darc/message_service.hpp>

#include <darc/id.hpp>

class PubSubTest : public darc::test::two_peer_sim, public testing::Test
{
public:
  darc::distributed_container::container_manager mngr1;
  darc::distributed_container::container_manager mngr2;
  darc::ns_service ns1;
  darc::ns_service ns2;

  PubSubTest() :
    mngr1(peer1),
    mngr2(peer2),
    ns1(peer1, &mngr1),
    ns2(peer2, &mngr2)
  {
  }
};

void handler(const boost::shared_ptr<const int> &data)
{
  std::cout << "DATA" << std::endl;
}

TEST_F(PubSubTest, PubSub)
{

  typedef darc::Publisher<int> MyPub;
  typedef darc::Subscriber<int> MySub;

  boost::asio::io_service io_service;

  darc::MessageService my_service1(peer1, io_service, ns1);
  MyPub test_pub(io_service, my_service1);

  darc::MessageService my_service2(peer2, io_service, ns2);
  MySub test_sub(io_service, my_service2);

  test_pub.attach("id1");
  test_sub.attach("id1");

//  my_service.nameserver_.registerPseudoTag("id1", "id2");

  ns1.connect(peer2.id());


  test_sub.addCallback(boost::bind(&handler, _1));

  boost::shared_ptr<int> data = boost::make_shared<int>(5);

  test_pub.publish(data);
  sleep(1);
  io_service.run();
  sleep(1);
  test_pub.publish(data);
  io_service.reset();
  io_service.run();
  sleep(1);
  test_pub.publish(data);
  io_service.reset();
  io_service.run();
  sleep(1);
  test_pub.publish(data);
  io_service.reset();
  io_service.run();

};
