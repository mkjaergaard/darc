#include <Python.h>
#include <boost/thread.hpp>
#include <darc/darc.h>

class IPythonComponent : public darc::Component
{
private:
  boost::thread python_thread_;

private:
  void runPyMain()
  {
    char * argv = "IPythonComponent";
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.argv = ['']");
    PyRun_SimpleString("from IPython.Shell import IPShellEmbed");
    PyRun_SimpleString("ipshell = IPShellEmbed()");
    PyRun_SimpleString("ipshell()");
    Py_Finalize();
  }

public:
  IPythonComponent()
  {
  }

  void onStart()
  {
    python_thread_ = boost::thread(boost::bind(&IPythonComponent::runPyMain, this));
  }

  void onStop()
  {
  }

};

DARC_REGISTER_COMPONENT(IPythonComponent)
