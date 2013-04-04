#include <gtest/gtest.h>

#include <darc/test/event_list.hpp>

#include <boost/asio.hpp>
#include <darc/network/network_manager.hpp>

void callback(darc::test::event_list* list, const std::string& event, const darc::ID& peer_id)
{
  list->event_callback(event, peer_id.short_string(), "");
}

TEST(NetworkTest, Connect)
{
  darc::test::event_list events;

  boost::asio::io_service io1; // not used by zmq

  darc::peer p1;
  p1.peer_connected_signal().connect(boost::bind(&callback, &events, "p1_connect", _1));
  p1.peer_disconnected_signal().connect(boost::bind(&callback, &events, "p1_disconnect", _1));
  darc::network::network_manager n1(io1, p1);
  n1.accept("zmq+tcp://127.0.0.1:5555");
  n1.connect("zmq+tcp://127.0.0.1:5556");
//  n1.connect("zmq+tcp://127.0.0.1:5557");

  std::cout << "peer1 " << p1.id().short_string() << std::endl;

  EXPECT_TRUE(events.is_empty());
  usleep(500*1000);
  EXPECT_TRUE(events.is_empty());

  std::cout << "Run 1" << std::endl;
  {
    boost::asio::io_service io2; // not used by zmq
    darc::peer p2;
    p2.peer_connected_signal().connect(boost::bind(&callback, &events, "p2_connect", _1));
    p2.peer_disconnected_signal().connect(boost::bind(&callback, &events, "p2_disconnect", _1));
    darc::network::network_manager n2(io2, p2);
    n2.accept("zmq+tcp://127.0.0.1:5556");
    usleep(1000*1000);
    n2.connect("zmq+tcp://127.0.0.1:5555");

    usleep(2000*1000);

    EXPECT_TRUE(events.pop_type("p1_connect"));
    EXPECT_TRUE(events.pop_type("p2_connect"));
    EXPECT_TRUE(events.is_empty());
  }
  usleep(1000*1000);
  EXPECT_TRUE(events.pop_type("p1_disconnect"));
  EXPECT_TRUE(events.is_empty());

  std::cout << "Run 2" << std::endl;
  {
    boost::asio::io_service io2; // not used by zmq
    darc::peer p2;
    p2.peer_connected_signal().connect(boost::bind(&callback, &events, "p2_connect", _1));
    p2.peer_disconnected_signal().connect(boost::bind(&callback, &events, "p2_disconnect", _1));
    darc::network::network_manager n2(io2, p2);
    n2.accept("zmq+tcp://127.0.0.1:5556");
    usleep(1000*1000);
    n2.connect("zmq+tcp://127.0.0.1:5555");

    usleep(2000*1000);

    EXPECT_TRUE(events.pop_type("p1_connect"));
    EXPECT_TRUE(events.pop_type("p2_connect"));
    EXPECT_TRUE(events.is_empty());
  }
  usleep(1000*1000);
  EXPECT_TRUE(events.pop_type("p1_disconnect"));
  EXPECT_TRUE(events.is_empty());

}
