#include <darc/local_tag.hpp>
#include <darc/ns_service.hpp>
#include <boost/make_shared.hpp>

namespace darc
{

local_tag::local_tag(ns_service * ns_service,
                     const std::string name) :
  ns_service_(ns_service),
  gen_(ID::null()),
  id_(gen_(name)),
  name_(name)
{
}

local_tag_ptr local_tag::create(ns_service * ns_service,
                                const std::string name)
{
  local_tag_ptr internal_tag = boost::make_shared<local_tag>(ns_service, name);
  ns_service->attach_tag(internal_tag);
  return local_tag_ptr(internal_tag.get(),
                       boost::bind(&ns_service::detach_tag, ns_service, _1));
}

local_tag::~local_tag()
{
}

}
