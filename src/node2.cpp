#include <iostream>
#include <darc/node.h>
#include <darc/component_register.h>

int main(int argc, const char* argv[])
{
  // Create Node
  darc::Node::Ptr node = darc::Node::create();

  // Create and run Component1
  darc::Component::Ptr c1 = darc::ComponentRegister::instantiateComponent( "Component2", node );
  boost::shared_ptr<boost::thread> c1_thread(new boost::thread( boost::bind(&darc::Component::run, c1)));

  // You can also manually construct a component and call the run() method if you want.
  // But using the register allows for other cool stuff. E.g. starting remotely.
  node->setNodeID(125);
  node->accept("udp://127.0.0.1:5006");
  //  node->remote_dispatch_handler_.connect(123, "udp://127.0.0.1:5006");

  // Run Node in main thread
  node->run();
  return 0;
}

