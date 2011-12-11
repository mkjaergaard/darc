#ifndef __DARC_PUBLISH_LOCAL_DISPATCHER_ABSTRACT_H_INCLUDED__
#define __DARC_PUBLISH_LOCAL_DISPATCHER_ABSTRACT_H_INCLUDED__

#include <darc/serialized_message.h>

namespace darc
{
namespace publish
{

class LocalDispatcherAbstract
{
public:
  virtual ~LocalDispatcherAbstract() {}

  virtual void dispatchMessageLocally( SerializedMessage::ConstPtr msg_s ) = 0;
};

}
}

#endif
