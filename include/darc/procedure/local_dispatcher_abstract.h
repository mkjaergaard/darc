#ifndef __DARC_PROCEDURE_LOCAL_DISPATCHER_ABSTRACT_INCLUDED_H__
#define __DARC_PROCEDURE_LOCAL_DISPATCHER_ABSTRACT_INCLUDED_H__

#include <boost/shared_ptr.hpp>

namespace darc
{
namespace procedure
{

class LocalDispatcherAbstract
{
public:
  typedef boost::shared_ptr<LocalDispatcherAbstract> Ptr;

  virtual ~LocalDispatcherAbstract()
  {
  }

};

}
}

#endif
