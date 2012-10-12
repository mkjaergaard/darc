#include <darc/local_tag.hpp>
#include <darc/ns_service.hpp>

namespace darc
{

local_tag::local_tag(ns_service * ns_service,
		     const std::string name,
		     const ID& parent_ns_id) :
  ns_service_(ns_service),
  id_(ID::create()),
  parent_ns_id_(parent_ns_id),
  name_(name)
{
  entry e(entry::tag_type);
  e.name = name;
  e.parent_id = parent_ns_id;
  ID id = ID::create();

  ns_service_->add_entry(id_, e);
}

local_tag::~local_tag()
{
  ns_service_->remove_entry(id_);
}

}
