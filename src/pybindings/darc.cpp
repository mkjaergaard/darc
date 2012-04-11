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
#include <darc/log.h>
#include <darc/component_loader.h>

namespace bp = boost::python;

namespace darc
{
namespace python
{

typedef boost::function<bp::object(boost::weak_ptr<darc::Primitive>)> ConvertToPythonMethod;
typedef std::map<int, ConvertToPythonMethod> ConverterListType;
ConverterListType converter_list_type_;

template<typename T, typename TP>
bp::object stdConverter(boost::weak_ptr<darc::Primitive> ptr)
{
  boost::shared_ptr<darc::Primitive> prim_shared = ptr.lock()->getWeakPtr().lock();
  boost::shared_ptr<T> type_shared = boost::dynamic_pointer_cast<T>(prim_shared);
  assert(type_shared.get());
  boost::weak_ptr<T> type_weak(type_shared);
  return bp::object(TP(type_weak));
}

void registerConverterMethod(int id, ConvertToPythonMethod method)
{
  converter_list_type_.insert(ConverterListType::value_type(id, method));
}

bp::object getPrimitiveObject(boost::weak_ptr<darc::Primitive> ptr)
{
  ConverterListType::iterator item = converter_list_type_.find(ptr.lock()->getTypeID());
  if(item != converter_list_type_.end())
  {
    return item->second(ptr);
  }
  else
  {
    std::cout << "Hmm" << std::endl;
    assert(false); // this fails if we dont have a proper converter registered
  }
}

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

  boost::python::object getValue()
  {
    // hack for the demo
    std::string my_string;
    int my_int;

    if(instance_.lock()->getValue2(my_string))
    {
      return boost::python::object(my_string);
    }
    else if(instance_.lock()->getValue2(my_int))
    {
      return boost::python::object(my_string);
    }
    else
    {
      return boost::python::object();
    }
  }

  void setValue(boost::python::object value)
  {
    // hack for the demo
    boost::python::extract<std::string> my_string(value);
    if (my_string.check())
    {
      std::string val = my_string;
      instance_.lock()->setValue(val);
      return;
    }
    boost::python::extract<int> my_int(value);
    if (my_int.check())
    {
      int val = my_int;
      instance_.lock()->setValue(val);
      return;
    }
  }


};

class ObjectList : public std::map<std::string, bp::object>
{
  typedef std::map<std::string, bp::object> MyType;

public:
  bp::object getitem(std::string text)
  {
    MyType::iterator item = this->find(text);
    if(item != this->end())
    {
      return item->second;
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
    for(MyType::iterator it = this->begin();
         it != this->end();
         it++)
    {
      l.insert(0, it->first);
    }
    return l;
  }
};

class OwnerProxy
{
protected:
  typedef std::map<std::string, int> TypeListType;

protected:
  boost::weak_ptr<darc::Owner> owner_instance_;
  TypeListType type_list_;

protected:
  void updateTypeList()
  {
    type_list_.clear();
    for(darc::Owner::PrimitiveListType::iterator it = owner_instance_.lock()->list_.begin();
	it != owner_instance_.lock()->list_.end();
	it++)
    {
      if(type_list_.find(it->second.lock()->getTypeName()) == type_list_.end())
      {
	type_list_.insert(TypeListType::value_type(it->second.lock()->getTypeName(), it->second.lock()->getTypeID()));
      }
    }
  }

public:
  OwnerProxy(boost::weak_ptr<darc::Owner> instance) :
    owner_instance_(instance)
  {
  }

  ObjectList getitem(std::string text)
  {
    updateTypeList();
    TypeListType::iterator item = type_list_.find(text);
    if(item != type_list_.end())
    {
      ObjectList l;
      for(darc::Owner::PrimitiveListType::iterator it = owner_instance_.lock()->list_.begin();
	  it != owner_instance_.lock()->list_.end();
	  it++)
      {
	int type_id = it->second.lock()->getTypeID();
	if(type_id == item->second)
	{
	  std::string name = it->second.lock()->getInstanceName();
	  if(name == "")
	  {
	    name = it->second.lock()->getTypeName();
	  }
	  l[name] = getPrimitiveObject(it->second);
	}
      }
      return l;
    }
    else
    {
      PyErr_SetString(PyExc_AttributeError, "object has no attribute with that name" );
      throw boost::python::error_already_set();
    }
  }

  boost::python::list dir()
  {
    updateTypeList();
    boost::python::list l;
    for(TypeListType::iterator it = type_list_.begin();
	it != type_list_.end();
	it++)
    {
      l.insert(0, it->first);
    }
    return l;
  }

};


class ComponentProxy : public OwnerProxy, public ProxyBase<darc::Component>
{
public:
  ComponentProxy(boost::weak_ptr<darc::Component> instance) :
    OwnerProxy(instance.lock()->getWeakPtr()),
    ProxyBase<darc::Component>(instance.lock()->getWeakPtr())
  {
  }

  std::string instanceName()
  {
    return instance_.lock()->getName();
  }

  void run()
  {
    return instance_.lock()->run();
  }

  boost::python::list dir()
  {
    return OwnerProxy::dir();
  }

  void pause() { instance_.lock()->pause(); }
  void unpause() { instance_.lock()->unpause(); }

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
  {
    for( NodeImpl::ComponentInstancesList::iterator it = instance_.lock()->component_instances_.begin();
	 it != instance_.lock()->component_instances_.end();
	 it++)
    {
      if(it->second->getName() == text)
      {
	return ComponentProxy(it->second);
      }
    }
    PyErr_SetString(PyExc_AttributeError, "object has no attribute with that name" );
    throw boost::python::error_already_set();
  }

  boost::python::list dir()
  {
    boost::python::list l;
    for( NodeImpl::ComponentInstancesList::iterator it = instance_.lock()->component_instances_.begin();
	 it != instance_.lock()->component_instances_.end();
	 it++)
    {
      l.insert(0, it->second->getName() );
    }
    return l;
  }
};


NodeProxy createNodeProxy( darc::NodeImpl* source)
{
  return NodeProxy(source->shared_from_this());
}

}
}


BOOST_PYTHON_MODULE(darc)
{
  darc::python::registerConverterMethod(123, boost::bind(&darc::python::stdConverter<darc::timer::PeriodicTimer, darc::python::PeriodicTimerProxy>, _1));
  darc::python::registerConverterMethod(234, boost::bind(&darc::python::stdConverter<darc::parameter::ParameterAbstract, darc::python::ParameterProxy>, _1));

  // DARC
  bp::class_<darc::Log, boost::noncopyable>("Log", bp::no_init)
    .add_static_property("level", &darc::Log::getLevel ,&darc::Log::setLevel)
    .def("logToFile", &darc::Log::logToFile)
    .def("logToConsole", &darc::Log::logToConsole)
    .staticmethod("logToFile")
    .staticmethod("logToConsole");

  bp::enum_<darc::Log::LevelType>("LevelType")
    .value("ALL", darc::Log::LOG_ALL)
    .value("TRACE", darc::Log::LOG_TRACE)
    .value("DEBUG", darc::Log::LOG_DEBUG)
    .value("INFO", darc::Log::LOG_INFO)
    .value("WARNING", darc::Log::LOG_WARNING)
    .value("ERROR", darc::Log::LOG_ERROR)
    .value("FATAL", darc::Log::LOG_FATAL);

  bp::class_<darc::Component, darc::ComponentPtr, boost::noncopyable>("Component", bp::no_init)
    .def("run", &darc::Component::run)
    .def("stop", &darc::Component::stop)
    .def("pause", &darc::Component::pause)
    .def("unpause", &darc::Component::unpause)
    .def("getName", &darc::Component::getName)
    .def("getID", &darc::Component::getID, bp::return_value_policy<bp::copy_const_reference>());

  bp::class_<darc::Registry>("Registry", bp::no_init)
    .def("instantiateComponent", &darc::Registry::instantiateComponent)
    .staticmethod("instantiateComponent");

  bp::class_<darc::ComponentLoader>("ComponentLoader", bp::no_init)
    .def("loadComponent", &darc::ComponentLoader::loadComponent)
    .staticmethod("loadComponent");

  bp::class_<darc::Node, darc::NodePtr, boost::noncopyable>("Node", bp::no_init)
    .def("create", &darc::Node::create)
    .staticmethod("create")
    .def("instantiateComponent", &darc::Node::instantiateComponent)
    .def("runComponent", &darc::Node::runComponent)
    .def("run", &darc::Node::run)
    .def("connect", &darc::Node::connect)
    .def("accept", &darc::Node::accept)
    .add_property("proxy", &darc::python::createNodeProxy);

  bp::class_<darc::NodeImpl, bp::bases<darc::Node>, boost::noncopyable>("NodeImpl", bp::no_init);

  // Proxys
  bp::class_<darc::python::ProxyBaseAbstract, boost::noncopyable >("Abstract_", bp::no_init );

  bp::class_<darc::python::OwnerProxy, boost::noncopyable>("Owner_", bp::no_init)
    .def("__dir__", &darc::python::OwnerProxy::dir)
    .def("__getattr__", &darc::python::OwnerProxy::getitem);

  bp::class_<darc::python::ComponentProxy, bp::bases<darc::python::OwnerProxy> >("Component_", bp::init<darc::ComponentPtr>())
    .def("instanceName", &darc::python::ComponentProxy::instanceName)
    .def("pause", &darc::python::ComponentProxy::pause)
    .def("unpause", &darc::python::ComponentProxy::unpause)
    .def("run", &darc::python::ComponentProxy::run)
    .def("__dir__", &darc::python::ComponentProxy::dir);


  bp::class_<darc::python::NodeProxy>("Node_", bp::init<boost::shared_ptr<darc::NodeImpl> >())
    .def("instantiateComponent", &darc::python::NodeProxy::instantiateComponent)
    .def("__dir__", &darc::python::NodeProxy::dir)
    .def("__getattr__", &darc::python::NodeProxy::getitem);

  bp::class_<darc::python::PeriodicTimerProxy>("PeriodicTimer_", bp::no_init)
    .add_property("period", &darc::python::PeriodicTimerProxy::getPeriod, &darc::python::PeriodicTimerProxy::setPeriod);

  bp::class_<darc::python::ParameterProxy>("Parameter_", bp::no_init)
    .add_property("name", &darc::python::ParameterProxy::getName)
    .add_property("value", &darc::python::ParameterProxy::getValue, &darc::python::ParameterProxy::setValue);

  bp::class_<darc::python::ObjectList>("ObjectList", bp::no_init)
    .def("__dir__", &darc::python::ObjectList::dir)
    .def("__getattr__", &darc::python::ObjectList::getitem);

  bp::class_<std::vector<std::string> >("list")
    .def(bp::vector_indexing_suite<std::vector<std::string> >());

}
