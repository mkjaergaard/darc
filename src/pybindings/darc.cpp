#include <iostream>
#include <boost/python.hpp>
#include <boost/thread.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/lexical_cast.hpp>

#include <darc/node.h>
#include <darc/node_impl.h>
#include <darc/registry.h>
#include <darc/component.h>
#include <darc/timer/periodic_timer.h>
#include <darc/python/proxy_base.h>

namespace bp = boost::python;
namespace xp = boost::xpressive;

namespace darc
{
namespace python
{

template<typename T, typename PT>
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
    xp::sregex rex( "timer_" >> (xp::s1= +xp::_d) );
    xp::smatch what;
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
      l.insert(0, std::string("timer_") + boost::lexical_cast<std::string>(i) );
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

typedef ItemListProxy<timer::PeriodicTimer, PeriodicTimerProxy> TimerListProxy;

class ComponentProxy : public ProxyBase<Component>
{
public:
  ComponentProxy(boost::shared_ptr<Component> instance) :
    ProxyBase(instance->shared_from_this())
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

};

}
}


BOOST_PYTHON_MODULE(darc)
{
  // DARC
  bp::class_<darc::Component, darc::ComponentPtr, boost::noncopyable>("Component", bp::no_init)
    .def("run", &darc::Component::run);

  bp::class_<darc::Registry>("Registry", bp::no_init)
    .def("instantiateComponent", &darc::Registry::instantiateComponent, bp::return_value_policy<bp::return_by_value>() )
    .staticmethod("instantiateComponent");

  bp::class_<darc::Node, darc::NodePtr, boost::noncopyable>("Node", bp::no_init)
    .def("create", &darc::Node::create)
    .staticmethod("create")
    .def("setNodeID", &darc::Node::setNodeID)
    .def("connect", &darc::Node::connect)
    .def("accept", &darc::Node::accept);

  bp::class_<darc::NodeImpl, bp::bases<darc::Node>, boost::noncopyable>("NodeImpl", bp::no_init);

  bp::class_<darc::python::ProxyBaseAbstract, boost::noncopyable >("Abstract_", bp::no_init );

  bp::class_<darc::python::ComponentProxy, boost::noncopyable >("Component_", bp::init<darc::ComponentPtr>())
    .def("instanceName", &darc::python::ComponentProxy::instanceName)
    .add_property("timers", &darc::python::ComponentProxy::timers);

  bp::class_<darc::python::PeriodicTimerProxy>("PeriodicTimer_", bp::no_init)
    .add_property("period", &darc::python::PeriodicTimerProxy::getPeriod, &darc::python::PeriodicTimerProxy::setPeriod);

  bp::class_<darc::python::TimerListProxy>("TimerList_", bp::no_init)
    .def("get", &darc::python::TimerListProxy::get)
    .def("__dir__", &darc::python::TimerListProxy::dir)
    .def("__getattribute__", &darc::python::TimerListProxy::getitem);

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
