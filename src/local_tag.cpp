#include <darc/local_tag.hpp>
#include <darc/local_ns.hpp>
#include <darc/ns_service.hpp>
#include <boost/make_shared.hpp>

namespace darc
{

local_tag::local_tag(ns_service * ns_service,
                     const std::string name,
                     local_ns * parent) :
  ns_service_(ns_service),
  parent_(parent),
  id_(ID::create()),
  name_(name)
{
}

local_tag_ptr local_tag::create(ns_service * ns_service,
                                const std::string name,
                                local_ns * parent)
{
  local_tag_ptr internal_tag = boost::make_shared<local_tag>(ns_service, name, parent);
  parent->attach_child_tag(internal_tag);
  return local_tag_ptr(internal_tag.get(),
                       boost::bind(&local_ns::detach_child_tag, parent, _1));
}

local_tag::~local_tag()
{
}

}
