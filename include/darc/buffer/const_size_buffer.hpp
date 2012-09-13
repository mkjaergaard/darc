#pragma once

#include <darc/buffer/raw_buffer.hpp>

namespace darc
{
namespace buffer
{

class const_size_buffer : public raw_buffer
{
public:
  const_size_buffer(size_t len) :
    raw_buffer(new char[len], len)
  {
  }

  ~const_size_buffer()
  {
    delete pbase();
  }

};

}
}
