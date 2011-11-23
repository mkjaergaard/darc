#include <iostream>
#include <darc/node.h>

int main(int argc, const char* argv[])
{
  // todo: figure out the best way to handle threads, starting components etc.
  //       right now they are started automatically in their own thread
  std::vector<boost::shared_ptr<boost::thread> > threads_;


  boost::asio::io_service io;
  darc::Node::Instance()->doSomeFun();
  while(1)
  {
    boost::asio::deadline_timer idle_timer(io, boost::posix_time::seconds(5));
    idle_timer.wait();
  }
  return 0;
}

