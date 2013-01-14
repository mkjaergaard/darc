#include <darc/entry.hpp>

namespace iris
{

template<>
std::string to_string(const arg<darc::entry>& e)
{
  return e->name;
}

}
