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

class BasicDispatcherGroup
{
public:
  virtual ~BasicDispatcherGroup()
  {
  }

  virtual void join(boost::shared_ptr<BasicDispatcherGroup> b_group) {};
  virtual void remote_message_recv(const ID& tag_id,
                                   darc::buffer::shared_buffer data) = 0;

};

template<typename T>
class DispatcherGroup : public BasicDispatcherGroup
{
private:
  typedef LocalDispatcher<T> DispatcherType;
  typedef boost::shared_ptr<DispatcherType> DispatcherPtr;

  typedef std::map<darc::ID, DispatcherPtr> DispatcherListType;

  DispatcherListType dispatcher_list_;

  tag_handle_impl::listener_type listener_;
  RemoteDispatcher * remote_dispatcher_;

public:
  DispatcherGroup(tag_handle_impl::listener_type listener, RemoteDispatcher * remote_dispatcher) :
    listener_(listener),
    remote_dispatcher_(remote_dispatcher)
  {
  }

  LocalDispatcher<T>* getDispatcher(const tag_handle& tag)
  {
    typename DispatcherListType::iterator elem = dispatcher_list_.find(tag->id());
    if(elem == dispatcher_list_.end())
    {
      boost::shared_ptr<LocalDispatcher<T> > dispatcher
        = boost::make_shared<LocalDispatcher<T> >(this, tag);

      dispatcher_list_.insert(
        typename DispatcherListType::value_type(tag->id(), dispatcher));

      // todo: put somewhere else?
      dispatcher->tag_->connect_listener(listener_);

      return dispatcher.get();
    }
    else
    {
      return elem->second.get();
    }
  }

  void join(boost::shared_ptr<BasicDispatcherGroup> b_group)
  {
    boost::shared_ptr<DispatcherGroup<T> > group
      = boost::dynamic_pointer_cast<DispatcherGroup<T> >(b_group);

    for(typename DispatcherListType::iterator it = group->dispatcher_list_.begin();
        it != group->dispatcher_list_.end();
        it++)
    {
      it->second->setGroup(this);
    }

    dispatcher_list_.insert(group->dispatcher_list_.begin(),
                            group->dispatcher_list_.end());

  }

  void dispatchToGroup(const ID& tag_id, const boost::shared_ptr<const T> &msg)
  {
    dispatchLocallyToGroup(tag_id, msg);
    remote_dispatcher_->dispatch_remotely(tag_id, msg);
  }

  void dispatchLocallyToGroup(const ID& tag_id, const boost::shared_ptr<const T> &msg)
  {
    for(typename DispatcherListType::iterator it = dispatcher_list_.begin();
        it != dispatcher_list_.end();
        it++)
    {
      it->second->dispatchLocally(msg);
    }
  }

  void remote_message_recv(const ID& tag_id,
                           darc::buffer::shared_buffer data)
  {
    inbound_data_ptr<serializer::boost_serializer, T> i_msg(data);
    dispatchLocallyToGroup(tag_id, i_msg.get());
  }

  // split

  // join
};

}
}
