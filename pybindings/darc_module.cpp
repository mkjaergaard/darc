#include <boost/python.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <darc/node.h>
#include <darc/node_impl.h>
#include <darc/component_register.h>
#include <darc/component.h>
#include <darc/timer/timer_list_ctrl_handle.h>
#include <darc/timer/periodic_timer_ctrl_handle.h>

#define SHOW() std::cout << __PRETTY_FUNCTION__ << "\n"

namespace bp = boost::python;

BOOST_PYTHON_MODULE(darc)
{
  SHOW();

  // DARC
  bp::class_<darc::Component, darc::ComponentPtr, boost::noncopyable>("Component", bp::no_init)
    .def("run", &darc::Component::run)
    .def("createCtrlHandle", &darc::Component::createCtrlHandle);

  bp::class_<darc::ComponentRegister>("ComponentRegister", bp::no_init)
    .def("instantiateComponent", &darc::ComponentRegister::instantiateComponent)
    .staticmethod("instantiateComponent");

  bp::class_<darc::Node, darc::NodePtr, boost::noncopyable>("Node", bp::no_init)
    .def("create", &darc::Node::create)
    .staticmethod("create")
    .def("setNodeID", &darc::Node::setNodeID)
    .def("connect", &darc::Node::connect)
    .def("accept", &darc::Node::accept);

  bp::class_<darc::NodeImpl, bp::bases<darc::Node>, boost::noncopyable>("NodeImpl", bp::no_init);
  // Ctrl Handles
  bp::class_<darc::ComponentCtrlHandle>("ComponentCtrlHandle", bp::no_init)
    .def("instanceName", &darc::ComponentCtrlHandle::instanceName)
    .add_property("timers", &darc::ComponentCtrlHandle::timers);

  bp::class_<darc::timer::TimerListCtrlHandle>("TimerListCtrlHandle", bp::no_init)
    .add_property("periodic", &darc::timer::TimerListCtrlHandle::getPeriodicTimers);

  bp::class_<darc::timer::PeriodicTimerCtrlHandle>("PeriodicTimerCtrlHandle", bp::no_init)
    .add_property("period", &darc::timer::PeriodicTimerCtrlHandle::getPeriod, &darc::timer::PeriodicTimerCtrlHandle::setPeriod);

  bp::class_<std::vector<darc::timer::PeriodicTimerCtrlHandle> >("PeriodicTimerCtrlHandleList")
    .def(bp::vector_indexing_suite<std::vector<darc::timer::PeriodicTimerCtrlHandle> >());

}
