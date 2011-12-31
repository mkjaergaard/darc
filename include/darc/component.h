#ifndef __DARC_COMPONENT_H_INCLUDED__
#define __DARC_COMPONENT_H_INCLUDED__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <darc/node.h>
#include <darc/owner.h>

namespace darc
{

class Component : public Owner
{
public:
  typedef boost::shared_ptr<Component> Ptr;

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
  // impl of darc::Owner
  boost::asio::io_service * getIOService()
  {
    return &io_service_;
  }

  // impl of darc::Owner
  boost::shared_ptr<Node> getNode()
  {
    return node_;
  }

  template<typename T>
  static boost::shared_ptr<T> instantiate( const std::string& instance_name, Node::Ptr node )
  {
    boost::shared_ptr<T> instance( new T(instance_name, node) );
    return instance;
  }

public:
  const std::string& getName() const
  {
    return name_;
  }

  void run()
  {
    std::cout << "Running Component: " << name_ << std::endl;
    boost::asio::io_service::work keep_alive(io_service_);
    io_service_.run();
    std::cout << "Component " << name_ << " Stopped!" << std::endl;
  }

};

}

// Include here so its available for the components
#include <darc/component_register.h>

#endif
