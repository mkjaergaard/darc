#pragma once

#include <hns/buffer.hpp>
#include <boost/asio/basic_streambuf.hpp>

namespace hns
{

class auto_buffer : public boost::asio::streambuf, public hns::buffer
{
public:
  virtual std::streambuf * streambuf()
  {
    return this;
  }

};

}
