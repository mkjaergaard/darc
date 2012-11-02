#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <darc/network/network_manager.hpp>

void callback1(const darc::ID& peer_id)
{
  std::cout << "C1: " << peer_id.short_string() << std::endl;
}

void callback2(const darc::ID& peer_id)
{
  std::cout << "C2: " << peer_id.short_string() << std::endl;
}

TEST(NetworkTest, Connect)
{

  boost::asio::io_service io1;
  darc::peer p1;
  p1.peer_connected_signal_.connect(boost::bind(&callback1, _1));
  darc::network::network_manager n1(io1, p1);
  n1.accept("zmq+tcp://127.0.0.1:5555");
  n1.connect("zmq+tcp://127.0.0.1:5556");

  boost::asio::io_service io2;
  darc::peer p2;
  p2.peer_connected_signal_.connect(boost::bind(&callback2, _1));
  darc::network::network_manager n2(io2, p2);
  n2.connect("zmq+tcp://127.0.0.1:5555");
  n2.accept("zmq+tcp://127.0.0.1:5556");

  sleep(1);

}
