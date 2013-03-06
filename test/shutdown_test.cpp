#include <boost/asio.hpp>
#include <darc/network/network_manager.hpp>

int main()
{
  boost::asio::io_service io1;
  darc::peer p1;
  darc::network::network_manager n1(io1, p1);
  n1.accept("zmq+tcp://127.0.0.1:5555");
  n1.connect("zmq+tcp://127.0.0.1:5556");
  usleep(100*1000);
  std::cout << "shutting down" << std::endl;
}
