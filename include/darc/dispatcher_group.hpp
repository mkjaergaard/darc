#pragma once

#include <boost/asio.hpp>
#include <darc/local_dispatcher.hpp>

#include <hns/tag_handle.hpp>

#include <beam/glog.hpp>

namespace darc
{

class BasicDispatcherGroup
{
public:
  virtual ~BasicDispatcherGroup()
  {
  }

  virtual void join(boost::shared_ptr<BasicDispatcherGroup> b_group) {};

};

template<typename T>
class DispatcherGroup : public BasicDispatcherGroup
{
private:
  typedef LocalDispatcher<T> DispatcherType;
  typedef boost::shared_ptr<DispatcherType> DispatcherPtr;

  typedef std::map<darc::ID, DispatcherPtr> DispatcherListType;

  DispatcherListType dispatcher_list_;

  hns::Tag::TagListenerType listener_;

public:
  DispatcherGroup(hns::Tag::TagListenerType listener) :
    listener_(listener)
  {
  }

  LocalDispatcher<T>* getDispatcher(const hns::TagHandle& tag)
  {
    typename DispatcherListType::iterator elem = dispatcher_list_.find(tag.id());
    if(elem == dispatcher_list_.end())
    {
      boost::shared_ptr<LocalDispatcher<T> > dispatcher
	= boost::make_shared<LocalDispatcher<T> >(this, tag);

      dispatcher_list_.insert(
	typename DispatcherListType::value_type(tag.id(), dispatcher));

      // todo: put somewhere else?
      dispatcher->tag_.addSubscriber(listener_);

      return dispatcher.get();
    }
    else
    {
      return elem->second.get();
    }
  }

  void join(boost::shared_ptr<BasicDispatcherGroup> b_group)
  {
    beam::glog<beam::Info>("joining");

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

  void dispatchToGroup(const boost::shared_ptr<const T> &msg)
  {
    beam::glog<beam::Info>("dispatchToGroup");
    for(typename DispatcherListType::iterator it = dispatcher_list_.begin();
	it != dispatcher_list_.end();
	it++)
    {
      beam::glog<beam::Info>("dispatchLocally");
      it->second->dispatchLocally(msg);
    }
  }


  // split

  // join
};

}
