#pragma once

#include <boost/asio.hpp>
#include <darc/pubsub/local_dispatcher.hpp>

#include <darc/tag_handle.hpp>
#include <darc/pubsub/remote_dispatcher.hpp>

#include <beam/glog.hpp>

#include <darc/inbound_data.hpp>
#include <darc/serializer/boost.hpp>

namespace darc
{
namespace pubsub
{

class basic_dispatcher_group
{
public:
  virtual ~basic_dispatcher_group()
  {
  }

  virtual void join(boost::shared_ptr<basic_dispatcher_group> b_group) {};
  virtual void remote_message_recv(const ID& tag_id,
                                   darc::buffer::shared_buffer data) = 0;

};

template<typename T>
class dispatcher_group : public basic_dispatcher_group
{
private:
  typedef local_dispatcher<T> dispatcher_type;
  typedef boost::shared_ptr<dispatcher_type> dispatcher_ptr;

  typedef std::map<darc::ID/*tag_id*/, dispatcher_ptr> dispatcher_list_type;

  dispatcher_list_type dispatcher_list_;
  remote_dispatcher * remote_dispatcher_;

public:
  dispatcher_group(remote_dispatcher * remote_dispatcher) :
    remote_dispatcher_(remote_dispatcher)
  {
  }

  ~dispatcher_group()
  {
    beam::glog<beam::Info>("~dispatcher_group");
  }

  local_dispatcher<T>* get_dispatcher(const tag_handle& tag)
  {
    typename dispatcher_list_type::iterator elem = dispatcher_list_.find(tag->id());
    if(elem == dispatcher_list_.end())
    {
      boost::shared_ptr<local_dispatcher<T> > dispatcher
        = boost::make_shared<local_dispatcher<T> >(this, tag);

      dispatcher_list_.insert(
        typename dispatcher_list_type::value_type(tag->id(), dispatcher));

      return dispatcher.get();
    }
    else
    {
      return elem->second.get();
    }
  }

  void remove_dispatcher(local_dispatcher<T>* dispatcher)
  {
    const ID& tag_id = dispatcher->tag_->id(); // todo: cleanup
    typename dispatcher_list_type::iterator elem = dispatcher_list_.find(tag_id);
    assert(elem != dispatcher_list_.end());
    dispatcher_list_.erase(elem);
  }

  void join(boost::shared_ptr<basic_dispatcher_group> b_group)
  {
    boost::shared_ptr<dispatcher_group<T> > group
      = boost::dynamic_pointer_cast<dispatcher_group<T> >(b_group);

    for(typename dispatcher_list_type::iterator it = group->dispatcher_list_.begin();
        it != group->dispatcher_list_.end();
        it++)
    {
      it->second->set_group(this);
    }

    dispatcher_list_.insert(group->dispatcher_list_.begin(),
                            group->dispatcher_list_.end());

  }

  void dispatch_to_group(const ID& tag_id, const boost::shared_ptr<const T> &msg)
  {
    dispatch_locally_to_group(tag_id, msg);
    remote_dispatcher_->dispatch_remotely(tag_id, msg);
  }

  void dispatch_locally_to_group(const ID& tag_id, const boost::shared_ptr<const T> &msg)
  {
    for(typename dispatcher_list_type::iterator it = dispatcher_list_.begin();
        it != dispatcher_list_.end();
        it++)
    {
      it->second->dispatch_locally(msg);
    }
  }

  void remote_message_recv(const ID& tag_id,
                           darc::buffer::shared_buffer data)
  {
    inbound_data_ptr<serializer::boost_serializer, T> i_msg(data);
    dispatch_locally_to_group(tag_id, i_msg.get());
  }

  // split

  // join
};

}
}
