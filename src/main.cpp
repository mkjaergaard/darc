#include <iostream>
#include <darc/node.h>

int main(int argc, const char* argv[])
{
  boost::shared_ptr<int> msg( new int() );
  std::string topic("test");
  darc::Node::Instance()->Publish(topic, msg );
  while(1);
  return 0;
}

