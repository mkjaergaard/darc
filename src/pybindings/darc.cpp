#include <iostream>
#include <boost/python.hpp>
#include <boost/thread.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <darc/node.h>
#include <darc/node_impl.h>
#include <darc/registry.h>
#include <darc/component.h>
#include <darc/timer/periodic_timer.h>
#include <darc/python/proxy_base.h>
#include <darc/parameter/parameter.h>

namespace bp = boost::python;

namespace darc
{
namespace python
{

template<typename T, typename PT, char const* myname>
class ItemListProxy : public ProxyBase< ItemList<T> >
{
  typedef ProxyBase< ItemList<T> > MyProxyBase;

public:
  ItemListProxy(boost::weak_ptr<ItemList<T> > instance) :
    MyProxyBase(instance)
  {
  }

  PT get(int idx)
  {
    return PT( MyProxyBase::instance_.lock()->list_.at(idx) );
  }

  PT getitem(std::string text)
  {
    std::string mynamestr(myname);
    boost::regex rex( mynamestr.append("(\\d+)") );
    boost::smatch what;
    if( regex_match(text, what, rex) )
    {
      int idx = boost::lexical_cast<int>(what[1]);
      return get(idx);
    }
    else
    {
      PyErr_SetString(PyExc_AttributeError, "object has no attribute with that name" );
      throw boost::python::error_already_set();
    }
  }

  boost::python::list dir()
  {
    boost::python::list l;
    for( unsigned int i = 0; i < MyProxyBase::instance_.lock()->list_.size(); i++ )
    {
      l.insert(0, std::string(myname) + boost::lexical_cast<std::string>(i) );
    }
    return l;
  }

};

class PeriodicTimerProxy : public ProxyBase<timer::PeriodicTimer>
{
public:
  PeriodicTimerProxy(boost::weak_ptr<timer::PeriodicTimer> instance) :
    ProxyBase(instance)
  {
  }

  double getPeriod()
  {
    return instance_.lock()->period_.total_milliseconds() / 1000.0;
  }

  void setPeriod(double new_period)
  {
    instance_.lock()->period_ = boost::posix_time::milliseconds( new_period * 1000 );
  }

};

class ParameterProxy : public ProxyBase<parameter::ParameterAbstract>
{
public:
  ParameterProxy(boost::weak_ptr<parameter::ParameterAbstract> instance) :
    ProxyBase(instance)
  {
  }

  std::string getName()
  {
    return instance_.lock()->getName();
  }

};

char timer_string[] = "timer_";
char parameter_string[] = "parameter_";

typedef ItemListProxy<timer::PeriodicTimer, PeriodicTimerProxy, timer_string> TimerListProxy;
typedef ItemListProxy<parameter::ParameterAbstract, ParameterProxy, parameter_string> ParameterListProxy;

class ComponentProxy : public ProxyBase<Component>
{
public:
  ComponentProxy(boost::weak_ptr<Component> instance) :
    ProxyBase(instance.lock()->getWeakPtr())
  {
  }

  std::string instanceName()
  {
    return instance_.lock()->getName();
  }

  TimerListProxy timers()
  {
    return TimerListProxy( instance_.lock()->timer_list_.getWeakPtr() );
  }

  ParameterListProxy parameters()
  {
    return ParameterListProxy( instance_.lock()->parameter_list_.getWeakPtr() );
  }

};

class NodeProxy : public ProxyBase<NodeImpl>
{
public:
  NodeProxy(boost::shared_ptr<NodeImpl> instance) :
    ProxyBase(instance->shared_from_this())
  {
  }

  void instantiateComponent(const std::string& instance_name)
  {
    instance_.lock()->instantiateComponent(instance_name);
  }

  ComponentProxy getitem(std::string text)
  {/*
    boost::regex rex( "component_(\\d+)" );
    boost::smatch what;
    if( regex_match(text, what, rex) )
    {
      return ComponentProxy( instance_.lock()->component_instances_[what[1]] );
    }
    else*/
    {
      PyErr_SetString(PyExc_AttributeError, "object has no attribute with that name" );
      throw boost::python::error_already_set();
      }
  }

  boost::python::list dir()
  {
    boost::python::list l;
    for( NodeImpl::ComponentInstancesList::iterator it = instance_.lock()->component_instances_.begin();
	 it != instance_.lock()->component_instances_.end();
	 it++)
    {
      //l.insert(0, std::string("component_") + it->first );
    }
    return l;
  }
};

}
}


BOOST_PYTHON_MODULE(darc)
{
  // DARC
  bp::class_<darc::Component, darc::ComponentPtr, boost::noncopyable>("Component", bp::no_init)
    .def("run", &darc::Component::run)
    .def("getName", &darc::Component::getName);

  bp::class_<darc::Registry>("Registry", bp::no_init)
    .def("instantiateComponent", &darc::Registry::instantiateComponent)
    .staticmethod("instantiateComponent");

  bp::class_<darc::Node, darc::NodePtr, boost::noncopyable>("Node", bp::no_init)
    .def("create", &darc::Node::create)
    .staticmethod("create")
    .def("instantiateComponent", &darc::Node::instantiateComponent)
    .def("setNodeID", &darc::Node::setNodeID)
    .def("connect", &darc::Node::connect)
    .def("accept", &darc::Node::accept);

  bp::class_<darc::NodeImpl, bp::bases<darc::Node>, boost::noncopyable>("NodeImpl", bp::no_init);


  // Proxys
  bp::class_<darc::python::ProxyBaseAbstract, boost::noncopyable >("Abstract_", bp::no_init );

  bp::class_<darc::python::ComponentProxy>("Component_", bp::init<darc::ComponentPtr>())
    .def("instanceName", &darc::python::ComponentProxy::instanceName)
    .add_property("timers", &darc::python::ComponentProxy::timers)
    .add_property("parameters", &darc::python::ComponentProxy::parameters);

  bp::class_<darc::python::NodeProxy>("Node_", bp::init<boost::shared_ptr<darc::NodeImpl> >())
    .def("instantiateComponent", &darc::python::NodeProxy::instantiateComponent)
    .def("__dir__", &darc::python::NodeProxy::dir)
    .def("__getattr__", &darc::python::NodeProxy::getitem);

  bp::class_<darc::python::PeriodicTimerProxy>("PeriodicTimer_", bp::no_init)
    .add_property("period", &darc::python::PeriodicTimerProxy::getPeriod, &darc::python::PeriodicTimerProxy::setPeriod);

  bp::class_<darc::python::ParameterProxy>("Parameter_", bp::no_init)
    .add_property("name", &darc::python::ParameterProxy::getName);

  bp::class_<darc::python::TimerListProxy>("TimerList_", bp::no_init)
    .def("get", &darc::python::TimerListProxy::get)
    .def("__dir__", &darc::python::TimerListProxy::dir)
    .def("__getattribute__", &darc::python::TimerListProxy::getitem);

  bp::class_<darc::python::ParameterListProxy>("ParameterList_", bp::no_init)
    .def("get", &darc::python::ParameterListProxy::get)
    .def("__dir__", &darc::python::ParameterListProxy::dir)
    .def("__getattribute__", &darc::python::ParameterListProxy::getitem);

  bp::class_<std::vector<std::string> >("list")
    .def(bp::vector_indexing_suite<std::vector<std::string> >());

  // Ctrl Handles
  /*  bp::class_<darc::ComponentCtrlHandle>("ComponentCtrlHandle", bp::no_init)
    .def("instanceName", &darc::ComponentCtrlHandle::instanceName)
    .add_property("timers", &darc::ComponentCtrlHandle::timers);

  bp::class_<darc::timer::TimerListCtrlHandle>("TimerListCtrlHandle", bp::no_init)
    .add_property("periodic", &darc::timer::TimerListCtrlHandle::getPeriodicTimers);

  bp::class_<darc::timer::PeriodicTimerCtrlHandle>("PeriodicTimerCtrlHandle", bp::no_init)
    .add_property("period", &darc::timer::PeriodicTimerCtrlHandle::getPeriod, &darc::timer::PeriodicTimerCtrlHandle::setPeriod);

  bp::class_<std::vector<darc::timer::PeriodicTimerCtrlHandle> >("PeriodicTimerCtrlHandleList")
    .def(bp::vector_indexing_suite<std::vector<darc::timer::PeriodicTimerCtrlHandle> >());
  */
}
