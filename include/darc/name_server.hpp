#pragma once

#include <darc/id.hpp>
#include <darc/peer_service.hpp>
#include <darc/distributed_container/shared_set.hpp>

namespace darc
{

struct namespace_entry
{
  ID parent;
  std::string name;
  ID namespace_list_id;
  ID tag_list_id;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & parent;
    ar & name;
    ar & namespace_list_id;
    ar & tag_list_id;
  }
};

struct tag_entry
{
  ID parent;
  std::string name;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & parent;
    ar & name;
  }
};

class ns_service : public darc::peer_service
{
protected:
  typedef distributed_container::shared_set<ID, namespace_entry> namespace_list_type;
  typedef boost::shared_ptr<namespace_list_type> namespace_list_ptr;
  typedef std::map<ID, namespace_list_ptr> namespace_list2_type;

  typedef distributed_container::shared_set<ID, tag_entry> tag_list_type;
  typedef boost::shared_ptr<tag_list_type> tag_list_ptr;
  typedef std::map<ID, tag_list_ptr> tag_list2_type;

  namespace_list2_type namespace_list;
  tag_list2_type tag_list;

  distributed_container::container_manager * container_manager_;

public:
  ID root_id_;
  namespace_entry root_entry_;

protected:
  namespace_list_ptr make_ns_entry_list()
  {
    namespace_list_ptr ns_entry_list = boost::make_shared<namespace_list_type>();
    ns_entry_list->attach(container_manager_);
    namespace_list.insert(
      namespace_list2_type::value_type(ns_entry_list->id(),
				       ns_entry_list));
    return ns_entry_list;
  }

  tag_list_ptr make_tag_entry_list()
  {
    tag_list_ptr tag_entry_list = boost::make_shared<tag_list_type>();
    tag_entry_list->attach(container_manager_);
    tag_list.insert(
      tag_list2_type::value_type(tag_entry_list->id(),
				 tag_entry_list));
    return tag_entry_list;
  }

  ///////////////////////
  // Debug thingy
public:
  void print_tree()
  {
    print_tree_(0, root_entry_.namespace_list_id);
  }

protected:
  void print_tree_(int depth, const ID& list_id)
  {
    namespace_list2_type::iterator item = namespace_list.find(list_id);
    assert(item != namespace_list.end());
    namespace_list_ptr list = item->second;

    // Recursive search
    for(namespace_list_type::iterator it = list->list().begin();
	it != list->list().end();
	it++)
    {
      std::cout << depth << " " << it->second.second.name << std::endl;
      print_tree_(depth+1, it->second.second.namespace_list_id);
    }
  }

  /////////////////////
  // Finding a NS-list. todo: we do it recursively now, not the most optimal way.
  namespace_list_ptr find_ns_list_(const ID& ns_id_find, const ID& ns_id, const ID& list_id)
  {
    namespace_list2_type::iterator item = namespace_list.find(list_id);
    assert(item != namespace_list.end());
    namespace_list_ptr list = item->second;

    // Check if we have the correct one
    if(ns_id_find == ns_id)
    {
      return list;
    }

    // Recursive search
    for(namespace_list_type::iterator it = list->list().begin();
	it != list->list().end();
	it++)
    {
      namespace_list_ptr ret =
	find_ns_list_(ns_id_find, it->first, it->second.second.namespace_list_id);
      if(ret.get() != 0)
      {
	return ret;
      }
    }

    // Return empty
    return namespace_list_ptr();

  }

  namespace_list_ptr get_ns_list(const ID& ns_id)
  {
    return find_ns_list_(ns_id, root_id_, root_entry_.namespace_list_id);
  }

  /////
  //////////////////////////

  // return namespace_list_ptr
  namespace_list_type::iterator has_ns(const ID& parent_ns_id,
				       namespace_list_ptr list,
				       const std::string& name)
  {
    for(namespace_list_type::iterator it = list->begin();
	it != list->end();
	it++)
    {
      if(it->second.second.name == name)
      {
	return it;
      }
    }
    return list->end();
  }

  namespace_list_type::iterator create_ns(const ID& parent_ns_id,
					  namespace_list_ptr parent,
					  const std::string& name)
  {
    namespace_list_type::iterator item = has_ns(parent_ns_id, parent, name);
    if(item != parent->end())
    {
      return item;
    }
    else
    {
      namespace_entry entry;
      entry.parent = parent_ns_id;
      entry.name = name;
      entry.namespace_list_id = make_ns_entry_list()->id();
      entry.tag_list_id = make_tag_entry_list()->id();
      ID ns_id = ID::create(); // todo: fix insert
      parent->insert(ns_id, entry);
      return parent->list().find(ns_id);
    }
  }

public:
  ns_service(distributed_container::container_manager * container_manager) :
    container_manager_(container_manager),
    root_id_(ID::create())
  {
    // Make root namespace
    root_entry_.parent = ID::null();
    root_entry_.name = "";
    root_entry_.namespace_list_id = make_ns_entry_list()->id();
    root_entry_.tag_list_id = make_tag_entry_list()->id();
  }

  // todo: the shared_set iterators are not correct iterators, since value has both owner+value
  const ID& register_namespace(const ID& parent_ns_id, const std::string& name)
  {
    namespace_list_ptr list = get_ns_list(parent_ns_id);
    return create_ns(parent_ns_id,
		     list,
		     name)->first;
  }

  void recv(const darc::ID& src_peer_id,
	    service_type,
	    darc::buffer::shared_buffer data)
  {
    assert(false);
  }


};

}
