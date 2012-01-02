#include <boost/python.hpp>
#include <boost/thread.hpp>
#include <iostream>

#define SHOW() std::cout << __PRETTY_FUNCTION__ << "\n"

struct Thing
{
  boost::thread t;

  bool running;
  unsigned n;
  std::string what;

  void bgthread()
  {
    n = 0;
    std::cout << "starting!\n";
    while(running || n % 15 != 0)
      {
        std::cout << what << "n=" << n++ << "\n";
        usleep(500000);
      }
    std::cout << "not running any more... exit!\n";
  }

  void start()
  {
    running = true;
    t = boost::thread(boost::bind(&Thing::bgthread, this));
  }

  void stop()
  {
    SHOW();
    running = false;
  }

  void wait()
  {
    t.join();
  }
};

namespace bp = boost::python;

BOOST_PYTHON_MODULE(darc)
{
  SHOW();

  bp::class_<Thing, boost::noncopyable>("Thing")
    .def("start", &Thing::start)
    .def("stop", &Thing::stop)
    .def("wait", &Thing::wait)
    .def_readwrite("what", &Thing::what)
    ;


}
