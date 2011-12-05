#ifndef __DARC_COMPONENT_REGISTER_H_INCLUDED_
#define __DARC_COMPONENT_REGISTER_H_INCLUDED_

#include <map>
#include <iostream>
#include <darc/component.h>
#include <darc/node.h>

namespace darc
{

class ComponentRegister
{
private:
  typedef boost::function<Component::Ptr(const std::string&, Node::Ptr)> InstantiateComponentMethod;
  typedef std::map<const std::string, InstantiateComponentMethod> ComponentListType;
  
  ComponentListType component_list_;

  static ComponentRegister * instance_;

private:
  ComponentRegister() {}

  static ComponentRegister * instance()
  {
    if( instance_ == 0 )
    {
      instance_ = new ComponentRegister();
    }
    return instance_;
  }

public:
  static int registerComponent( const std::string& component_name, InstantiateComponentMethod method )
  {
    ComponentRegister * inst = instance();
    inst->component_list_[component_name] = method;
    std::cout << "Registered Component: " << component_name << std::endl;
    return 1;
  }

  static darc::Component::Ptr instantiateComponent( const std::string& instance_name, Node::Ptr node )
  {
    ComponentRegister * inst = instance();
    if( inst->component_list_.count(instance_name) )
    {
      return inst->component_list_[instance_name](instance_name, node);
    }
    else
    {
      assert(0);
      darc::Component::Ptr n;
      return n;
    }
  }

};

}

#endif
