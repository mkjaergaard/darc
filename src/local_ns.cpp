#include <darc/local_ns.hpp>
#include <darc/ns_service.hpp>

namespace darc
{

local_ns::local_ns(ns_service * service,
		   distributed_container::container_manager * container_manager,
		   const std::string name,
		   local_ns* parent):
  ns_service_(service),
  parent_(parent),
  id_(ID::create()),
  name_(name)
{
  list_.attach(container_manager);
  list_.signal_.connect(boost::bind(&local_ns::callback, this, _1, _2, _3, _4));
}

local_ns_ptr local_ns::create(ns_service * ns_service,
			      distributed_container::container_manager * container_manager,
			      const std::string name,
			      local_ns * parent)
{
  local_ns_ptr ns = boost::make_shared<local_ns>(ns_service,
						 container_manager,
						 name,
						 parent);
  if(parent != 0)
  {
    parent->attach_child_ns(ns);
  }
  return ns;
}

}
