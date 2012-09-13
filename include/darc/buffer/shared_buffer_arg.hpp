#pragma once

#include <hns/shared_buffer.hpp>

namespace llog
{

template<>
std::string to_string(const Argument<hns::shared_buffer>& buffer)
{
  std::ostringstream os;
  os << std::hex;
  const char* data = (*buffer)->data();
  for(size_t i = 0;
      i < 100;
      i++)
  {
    if(data[i] < 16)
    {
      os << "0";
    }
    os << (int)data[i];
    os << ":";
  }
  return os.str();
}

}
