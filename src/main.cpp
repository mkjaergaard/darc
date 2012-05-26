#include <asmsg/publisher.hpp>
#include <asmsg/subscriber.hpp>
#include <asmsg/local_dispatcher.hpp>
#include <asmsg/message_service.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

typedef asmsg::Publisher<int, boost::uuids::uuid> MyPub;
typedef asmsg::Subscriber<int, boost::uuids::uuid> MySub;

void handler(const boost::shared_ptr<const int> &data)
{
  std::cout << "hej" << std::endl;
}

int main()
{
  boost::uuids::uuid id1 = boost::uuids::random_generator()();
  asmsg::MessageService<boost::uuids::uuid> my_service;
  boost::asio::io_service io_service;

  MyPub test_pub(io_service, my_service);
  MySub test_sub(io_service, my_service);

  test_pub.attach(id1);
  test_sub.attach(id1);

  test_sub.addCallback(boost::bind(&handler, _1));

  boost::shared_ptr<int> data = boost::make_shared<int>(5);

  test_pub.publish(data);

  io_service.run();

}
