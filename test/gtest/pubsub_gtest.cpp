#include <gtest/gtest.h>

#include <darc/test/two_peer_sim.hpp>
#include <darc/primitives/pubsub/publisher.hpp>
#include <darc/primitives/pubsub/subscriber.hpp>
#include <darc/primitives/pubsub/message_service.hpp>

#include <darc/id.hpp>

class PubSubTest : public darc::test::two_peer_sim, public testing::Test
{
public:
  darc::ns_service ns1;
  darc::ns_service ns2;

  PubSubTest() :
    ns1(peer1),
    ns2(peer2)
  {
  }
};

void handler(const int& data)
{
  std::cout << "DATA" << std::endl;
}

TEST_F(PubSubTest, PubSub)
{
  typedef darc::pubsub::publisher<int> MyPub;
  typedef darc::pubsub::subscriber<int> MySub;

  boost::asio::io_service io_service;

  darc::pubsub::message_service my_service1(peer1, io_service, ns1);
  MyPub test_pub(io_service, my_service1);

  darc::pubsub::message_service my_service2(peer2, io_service, ns2);
  MySub test_sub(io_service, my_service2);

  test_pub.attach("id1");
  test_sub.attach("id1");

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
