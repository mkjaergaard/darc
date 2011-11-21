#ifndef __DARC_COMPONENT_H_INCLUDED__
#define __DARC_COMPONENT_H_INCLUDED__


#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace darc
{

class Node;

class Component
{
 protected:
  std::string name_;
  boost::shared_ptr<Node> node_;
  boost::asio::io_service io_service_;

 protected:
 Component(const std::string& name, boost::shared_ptr<Node> node):
  name_(name),
  node_( node )
  {
  }

 public:
  boost::asio::io_service * GetIOService()
  {
    return &io_service_;
  }

  boost::shared_ptr<Node> GetNode()
  {
    return node_;
  }

 public:
  const std::string& getName() const
  {
    return name_;
  }

  void run()
  {
    std::cout << "Running Component: " << name_ << std::endl;
    io_service_.run();
  }
  
};
}

#endif
