#include <gtest/gtest.h>

#include <darc/publisher.hpp>
#include <darc/subscriber.hpp>
#include <darc/local_dispatcher.hpp>
#include <darc/message_service.hpp>

#include <darc/id.hpp>

class PubSubTest : public testing::Test
{
public:
//  boost::asio::io_service io_service_;

  darc::distributed_container::container_manager mngr1;
  darc::distributed_container::container_manager mngr2;
  darc::ID node1_id;
  darc::ID node2_id;
  darc::ns_service ns1;
  darc::ns_service ns2;

  PubSubTest() :
    mngr1(boost::bind(&PubSubTest::send_to_node2_, this, _1, _2)),
    mngr2(boost::bind(&PubSubTest::send_to_node1_, this, _1, _2)),
    node1_id(darc::ID::create()),
    node2_id(darc::ID::create()),
    ns1(&mngr1),
    ns2(&mngr2)
  {
    ns1.set_send_to_function(
      boost::bind(&PubSubTest::send_to_node2, this, _1, _2, _3));
    ns2.set_send_to_function(
      boost::bind(&PubSubTest::send_to_node1, this, _1, _2, _3));
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

void handler(const boost::shared_ptr<const int> &data)
{
  std::cout << "DATA" << std::endl;
}

TEST_F(PubSubTest, PubSub)
{
  typedef darc::Publisher<int> MyPub;
  typedef darc::Subscriber<int> MySub;

  darc::ID id1 = darc::ID::create();
  boost::asio::io_service io_service;

  darc::MessageService my_service(io_service, ns1);
  MyPub test_pub(io_service, my_service);
  MySub test_sub(io_service, my_service);

  test_pub.attach("id1");
  test_sub.attach("id2");

//  my_service.nameserver_.registerPseudoTag("id1", "id2");

  test_sub.addCallback(boost::bind(&handler, _1));

  boost::shared_ptr<int> data = boost::make_shared<int>(5);

  test_pub.publish(data);

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
