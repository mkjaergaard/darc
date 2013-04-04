#pragma once

#include <hns/buffer.hpp>
#include <boost/asio/streambuf.hpp>

namespace hns
{

class asio_buffer : public boost::asio::streambuf, public hns::buffer
{
public:
  virtual std::streambuf * streambuf()
  {
    return this;
  }

};

}
