#ifndef __DARC_SUBCOMPONENT_H_INCLUDED__
#define __DARC_SUBCOMPONENT_H_INCLUDED__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <darc/node.h>
#include <darc/component.h>

namespace darc
{

class Subcomponent : public Owner
{
protected:
  Component * parent_;

protected:
  Subcomponent(Component * parent):
    parent_(parent)
  {
  }

  virtual ~Subcomponent()
  {
  }

public:
  // impl of darc::Owner
  boost::asio::io_service * getIOService()
  {
    return parent_->getIOService();
  }

  // impl of darc::Owner
  boost::shared_ptr<Node> getNode()
  {
    return parent_->getNode();
  }

};

}

#endif
