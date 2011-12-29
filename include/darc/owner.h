#ifndef __DARC_OWNER_H_INCLUDED__
#define __DARC_OWNER_H_INCLUDED__

#include <boost/asio.hpp>
#include <darc/node.h>

namespace darc
{

class Owner
{
protected:

public:
  virtual boost::asio::io_service * getIOService() = 0;
  virtual boost::shared_ptr<Node> getNode() = 0;
};

}

#endif
