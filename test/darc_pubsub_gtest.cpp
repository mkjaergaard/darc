#include <gtest/gtest.h>

#include <darc/publisher.hpp>
#include <darc/subscriber.hpp>
#include <darc/local_dispatcher.hpp>
#include <darc/message_service.hpp>

#include <darc/id.hpp>

void handler(const boost::shared_ptr<const int> &data)
{
  std::cout << "hej" << std::endl;
}

TEST(darcTest, PubSub)
{
  typedef darc::Publisher<int> MyPub;
  typedef darc::Subscriber<int> MySub;

  darc::ID id1 = darc::ID::create();
  boost::asio::io_service io_service;

  darc::MessageService my_service(io_service);
  MyPub test_pub(io_service, my_service);
  MySub test_sub(io_service, my_service);

  test_pub.attach("id1");
  test_sub.attach("id2");

  my_service.nameserver_.registerPseudoTag("id1", "id2");

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
