#include <boost/python.hpp>
#include <boost/thread.hpp>
#include <iostream>

#include <darc/node.h>
#include <darc/node_impl.h>
#include <darc/component_register.h>
#include <darc/component.h>

#define SHOW() std::cout << __PRETTY_FUNCTION__ << "\n"

namespace bp = boost::python;

BOOST_PYTHON_MODULE(darc)
{
  SHOW();

  bp::class_<darc::Component, darc::ComponentPtr, boost::noncopyable>("Component", bp::no_init)
    .def("run", &darc::Component::run)
    .def("createCtrlHandle", &darc::Component::createCtrlHandle);

  bp::class_<darc::ComponentCtrlHandle>("ComponentCtrlHandle", bp::no_init)
    .def("getInstanceName", &darc::ComponentCtrlHandle::getInstanceName);

  bp::class_<darc::ComponentRegister>("ComponentRegister", bp::no_init)
    .def("instantiateComponent", &darc::ComponentRegister::instantiateComponent)
    .staticmethod("instantiateComponent");

  bp::class_<darc::Node, darc::NodePtr, boost::noncopyable>("Node", bp::no_init)
    .def("create", &darc::Node::create)
    .staticmethod("create");

  bp::class_<darc::NodeImpl, boost::noncopyable>("NodeImpl", bp::no_init);

}
